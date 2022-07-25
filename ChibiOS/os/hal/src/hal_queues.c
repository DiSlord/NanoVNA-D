/*
    ChibiOS - Copyright (C) 2006..2016 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/**
 * @file    hal_queues.c
 * @brief   I/O Queues code.
 *
 * @addtogroup HAL_QUEUES
 * @details Queues are mostly used in serial-like device drivers.
 *          Serial device drivers are usually designed to have a lower side
 *          (lower driver, it is usually an interrupt service routine) and an
 *          upper side (upper driver, accessed by the application threads).<br>
 *          There are several kind of queues:<br>
 *          - <b>Input queue</b>, unidirectional queue where the writer is the
 *            lower side and the reader is the upper side.
 *          - <b>Output queue</b>, unidirectional queue where the writer is the
 *            upper side and the reader is the lower side.
 *          - <b>Full duplex queue</b>, bidirectional queue. Full duplex queues
 *            are implemented by pairing an input queue and an output queue
 *            together.
 *          .
 * @{
 */

#include "hal.h"
#include "string.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/
/* Inform the low side that the queue has at least one slot available.*/
static inline void q_Notify(input_queue_t *qp) {
  if (qp->q_notify != NULL) qp->q_notify(qp);
}

/* Restore suspend by waitReadyTimeout or waitFreeTimeout thread */
static inline void q_Restore(input_queue_t *qp) {
  osalThreadDequeueNextI(&qp->q_waiting, MSG_OK);
}

/* Waiting until there empty or a timeout occurs.*/
static msg_t waitReadyTimeout(io_queue_t *qp, systime_t timeout) {
  if (qIsEmptyI(qp))
    return  osalThreadEnqueueTimeoutS(&qp->q_waiting, timeout);
  return MSG_OK;
}

/* Waiting until there is a slot available or a timeout occurs.*/
static msg_t waitFreeTimeout(io_queue_t *qp, systime_t timeout) {
  if (qIsFullI(qp))
    return osalThreadEnqueueTimeoutS(&qp->q_waiting, timeout);
  return MSG_OK;
}

/* Getting the character from the queue.*/
static msg_t q_GetI(io_queue_t *qp) {
  qp->q_counter--;
  uint8_t b = qp->q_buffer[qp->q_rd++];
  if (qp->q_rd >= qp->q_size) qp->q_rd = 0;

  return (msg_t)b;
}

/* Putting the character into the queue.*/
static void q_PutI(io_queue_t *qp, uint8_t b) {
  qp->q_counter++;
  qp->q_buffer[qp->q_wr++] = b;
  if (qp->q_wr >= qp->q_size) qp->q_wr = 0;
}

/**
 * @brief   Non-blocking input queue read.
 * @details The function reads data from an input queue into a buffer. The
 *          operation completes when the specified amount of data has been
 *          transferred or when the input queue has been emptied.
 *
 * @param[in] iqp       pointer to an @p input_queue_t structure
 * @param[out] bp       pointer to the data buffer
 * @param[in] n         the maximum amount of data to be transferred, the
 *                      value 0 is reserved
 * @return              The number of bytes effectively transferred.
 *
 * @notapi
 */
static qsize_t q_read(input_queue_t *iqp, uint8_t *bp, qsize_t n) {
  osalDbgCheck(n > 0U);
  /* Get avaible data count */
  qsize_t s = qGetFullI(iqp);
  /* Number of bytes that can be read in a single atomic operation.*/
  if (n > s) n = s;
  if (n == 0) return 0;
  if (n == 1) {
    *bp = q_GetI(iqp);
    return 1;
  }
  /* Number of bytes before buffer limit.*/
  s = iqp->q_size - iqp->q_rd;
  if (n < s) {
    memcpy((void *)&bp[0], (void *)&iqp->q_buffer[iqp->q_rd],     n);
  } else {
    memcpy((void *)&bp[0], (void *)&iqp->q_buffer[iqp->q_rd],     s);
    memcpy((void *)&bp[s], (void *)&iqp->q_buffer[0        ], n - s);
    iqp->q_rd = -s;
  }
  iqp->q_rd      += n;
  iqp->q_counter -= n;
  return n;
}

/**
 * @brief   Non-blocking output queue write.
 * @details The function writes data from a buffer to an output queue. The
 *          operation completes when the specified amount of data has been
 *          transferred or when the output queue has been filled.
 *
 * @param[in] oqp       pointer to an @p output_queue_t structure
 * @param[in] bp        pointer to the data buffer
 * @param[in] n         the maximum amount of data to be transferred, the
 *                      value 0 is reserved
 * @return              The number of bytes effectively transferred.
 *
 * @notapi
 */
static qsize_t q_write(output_queue_t *oqp, const uint8_t *bp, qsize_t n) {
  osalDbgCheck(n > 0U);
  qsize_t s = qGetEmptyI(oqp);
  /* Number of bytes that can be written in a single atomic operation.*/
  if (n > s) n = s;
  if (n == 0) return 0;
  if (n == 1) {
    q_PutI(oqp, *bp);
    return 1;
  }
  /* Number of bytes before buffer limit.*/
  s = oqp->q_size - oqp->q_wr;
  if (n < s) {
    memcpy((void *)&oqp->q_buffer[oqp->q_wr], (const void *)&bp[0],     n);
  } else {
    memcpy((void *)&oqp->q_buffer[oqp->q_wr], (const void *)&bp[0],     s);
    memcpy((void *)&oqp->q_buffer[0        ], (const void *)&bp[s], n - s);
    oqp->q_wr = -s;
  }
  oqp->q_wr      += n;
  oqp->q_counter += n;
  return n;
}

/**
 * @brief   Initializes an input queue.
 * @details A Semaphore is internally initialized and works as a counter of
 *          the bytes contained in the queue.
 * @note    The callback is invoked from within the S-Locked system state.
 *
 * @param[out] iqp      pointer to an @p input_queue_t structure
 * @param[in] bp        pointer to a memory area allocated as queue buffer
 * @param[in] size      size of the queue buffer
 * @param[in] infy      pointer to a callback function that is invoked when
 *                      data is read from the queue. The value can be @p NULL.
 * @param[in] link      application defined pointer
 *
 * @init
 */
void qObjectInit(io_queue_t *iqp, uint8_t *bp, qsize_t size, qnotify_t infy) {
  osalThreadQueueObjectInit(&iqp->q_waiting);
  iqp->q_counter = 0;
  iqp->q_buffer  = bp;
  iqp->q_rd      = 0;
  iqp->q_wr      = 0;
  iqp->q_size    = size;
  iqp->q_notify  = infy;
}

/**
 * @brief   Resets an input queue.
 * @details All the data in the input queue is erased and lost, any waiting
 *          thread is resumed with status @p MSG_RESET.
 * @note    A reset operation can be used by a low level driver in order to
 *          obtain immediate attention from the high level layers.
 *
 * @param[in] iqp       pointer to an @p input_queue_t structure
 *
 * @iclass
 */
void qResetI(io_queue_t *iqp) {
  osalDbgCheckClassI();
  iqp->q_rd      = 0;
  iqp->q_wr      = 0;
  iqp->q_counter = 0;
  osalThreadDequeueAllI(&iqp->q_waiting, MSG_RESET);
}

/**
 * @brief   Input queue write.
 * @details A byte value is written into the low end of an input queue.
 *
 * @param[in] iqp       pointer to an @p input_queue_t structure
 * @param[in] b         the byte value to be written in the queue
 * @return              The operation status.
 * @retval MSG_OK       if the operation has been completed with success.
 * @retval MSG_TIMEOUT  if the queue is full and the operation cannot be
 *                      completed.
 *
 * @iclass
 */
msg_t qPutI(io_queue_t *iqp, uint8_t b) {
  osalDbgCheckClassI();
  if (qIsFullI(iqp)) {
    return MSG_TIMEOUT;
  }
  q_PutI(iqp, b);
  q_Restore(iqp);
  return MSG_OK;
}

/**
 * @brief   Input queue read with timeout.
 * @details This function reads a byte value from an input queue. If the queue
 *          is empty then the calling thread is suspended until a byte arrives
 *          in the queue or a timeout occurs.
 * @note    The callback is invoked after removing a character from the
 *          queue.
 *
 * @param[in] iqp       pointer to an @p input_queue_t structure
 * @param[in] timeout   the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_IMMEDIATE immediate timeout.
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 * @return              A byte value from the queue.
 * @retval MSG_TIMEOUT  if the specified time expired.
 * @retval MSG_RESET    if the queue has been reset.
 *
 * @api
 */
msg_t iqGetTimeout(input_queue_t *iqp, systime_t timeout) {
  osalSysLock();
  /* Waiting until there is a character available or a timeout occurs.*/
  msg_t msg = waitReadyTimeout(iqp, timeout);
  if (msg == MSG_OK) {
    /* Getting the character from the queue.*/
    msg = q_GetI(iqp);
    /* Inform the low side that the queue has at least one slot available.*/
    q_Notify(iqp);
  }
  osalSysUnlock();
  return msg;
}

/**
 * @brief   Input queue read with timeout.
 * @details The function reads data from an input queue into a buffer. The
 *          operation completes when the specified amount of data has been
 *          transferred or after the specified timeout or if the queue has
 *          been reset.
 * @note    The function is not atomic, if you need atomicity it is suggested
 *          to use a semaphore or a mutex for mutual exclusion.
 * @note    The callback is invoked after removing each character from the
 *          queue.
 *
 * @param[in] iqp       pointer to an @p input_queue_t structure
 * @param[out] bp       pointer to the data buffer
 * @param[in] n         the maximum amount of data to be transferred, the
 *                      value 0 is reserved
 * @param[in] timeout   the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_IMMEDIATE immediate timeout.
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 * @return              The number of bytes effectively transferred.
 *
 * @api
 */
qsize_t iqReadTimeout(input_queue_t *iqp, uint8_t *bp, qsize_t n, systime_t timeout) {
  qsize_t r = 0;
  osalDbgCheck(n > 0U);
  osalSysLock();
  while (waitReadyTimeout(iqp, timeout) == MSG_OK) {
    qsize_t done = q_read(iqp, &bp[r], n - r);
    /* Inform the low side that the queue has at least one empty slot available.*/
    q_Notify(iqp);
    r += done;
    if (r >= n) break;
  }
  osalSysUnlock();
  return r;
}

/**
 * @brief   Output queue write with timeout.
 * @details This function writes a byte value to an output queue. If the queue
 *          is full then the calling thread is suspended until there is space
 *          in the queue or a timeout occurs.
 * @note    The callback is invoked after putting the character into the
 *          queue.
 *
 * @param[in] oqp       pointer to an @p output_queue_t structure
 * @param[in] b         the byte value to be written in the queue
 * @param[in] timeout   the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_IMMEDIATE immediate timeout.
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 * @return              The operation status.
 * @retval MSG_OK       if the operation succeeded.
 * @retval MSG_TIMEOUT  if the specified time expired.
 * @retval MSG_RESET    if the queue has been reset.
 *
 * @api
 */
msg_t oqPutTimeout(output_queue_t *oqp, uint8_t b, systime_t timeout) {
  osalSysLock();
  /* Waiting until there is a slot available or a timeout occurs.*/
  msg_t msg = waitFreeTimeout(oqp, timeout);
  if (msg == MSG_OK) {
    /* Putting the character into the queue.*/
    q_PutI(oqp, b);
    /* Inform the low side that the queue has at least one character available.*/
    q_Notify(oqp);
  }
  osalSysUnlock();
  return msg;
}

/**
 * @brief   Output queue read.
 * @details A byte value is read from the low end of an output queue.
 *
 * @param[in] oqp       pointer to an @p output_queue_t structure
 * @return              The byte value from the queue.
 * @retval MSG_TIMEOUT  if the queue is empty.
 *
 * @iclass
 */
msg_t qGetI(io_queue_t *oqp) {
  osalDbgCheckClassI();
  if (qIsEmptyI(oqp))
    return MSG_TIMEOUT;
  msg_t b = q_GetI(oqp);
  q_Restore(oqp); // Restore on last byte
  return b;
}

/**
 * @brief   Output queue write with timeout.
 * @details The function writes data from a buffer to an output queue. The
 *          operation completes when the specified amount of data has been
 *          transferred or after the specified timeout or if the queue has
 *          been reset.
 * @note    The function is not atomic, if you need atomicity it is suggested
 *          to use a semaphore or a mutex for mutual exclusion.
 * @note    The callback is invoked after putting each character into the
 *          queue.
 *
 * @param[in] oqp       pointer to an @p output_queue_t structure
 * @param[in] bp        pointer to the data buffer
 * @param[in] n         the maximum amount of data to be transferred, the
 *                      value 0 is reserved
 * @param[in] timeout   the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_IMMEDIATE immediate timeout.
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 * @return              The number of bytes effectively transferred.
 *
 * @api
 */
qsize_t oqWriteTimeout(output_queue_t *oqp, const uint8_t *bp, qsize_t n, systime_t timeout) {
  qsize_t w = 0;
  osalDbgCheck(n > 0U);
  osalSysLock();
  while (waitFreeTimeout(oqp, timeout) == MSG_OK) {
    qsize_t done = q_write(oqp, &bp[w], n - w);
    /* Inform the low side that the queue has at least one character available.*/
    q_Notify(oqp);
    w += done;
    if (w >= n) break;
  }
  osalSysUnlock();
  return w;
}
/** @} */

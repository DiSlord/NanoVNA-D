/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

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
 * @file    hal_buffers.c
 * @brief   I/O Buffers code.
 *
 * @addtogroup HAL_BUFFERS
 * @details Buffers Queues are used when there is the need to exchange
 *          fixed-length data buffers between ISRs and threads.
 *          On the ISR side data can be exchanged only using buffers,
 *          on the thread side data can be exchanged both using buffers and/or
 *          using an emulation of regular byte queues.
 *          There are several kind of buffers queues:<br>
 *          - <b>Input queue</b>, unidirectional queue where the writer is the
 *            ISR side and the reader is the thread side.
 *          - <b>Output queue</b>, unidirectional queue where the writer is the
 *            thread side and the reader is the ISR side.
 *          - <b>Full duplex queue</b>, bidirectional queue. Full duplex queues
 *            are implemented by pairing an input queue and an output queue
 *            together.
 *          .
 * @{
 */

#include <string.h>

#include "hal.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/* Inform the low side that the queue has at least one slot available.*/
static inline void bq_Notify(io_buffers_queue_t *bqp) {
  if (bqp->notify != NULL) bqp->notify(bqp);
}

/* Restore suspend by waitReadyTimeout or waitFreeTimeout thread */
static inline void bq_Restore(io_buffers_queue_t *bqp) {
  osalThreadDequeueNextI(&bqp->waiting, MSG_OK);
}

/* Waiting until there empty or a timeout occurs.*/
static msg_t waitReadyTimeout(io_buffers_queue_t *bqp, systime_t timeout) {
  if (bqIsEmptyI(bqp))
    return  osalThreadEnqueueTimeoutS(&bqp->waiting, timeout);
  return MSG_OK;
}

/* Waiting until there is a slot available or a timeout occurs.*/
static msg_t waitFreeTimeout(io_buffers_queue_t *bqp, systime_t timeout) {
  if (bqIsFullI(bqp))
    return osalThreadEnqueueTimeoutS(&bqp->waiting, timeout);
  return MSG_OK;
}

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/
static size_t bqCopyTimeout(io_buffers_queue_t *bqp, bool dir, uint8_t *bp, size_t n, systime_t timeout) {
  size_t c = 0, size;

  osalSysLock();
  do {
    /* This condition indicates that a new buffer must be acquired.*/
    if (bqp->ptr == NULL) {
      msg_t msg = dir ? obqGetEmptyBufferTimeoutS(bqp, timeout) :
                        ibqGetFullBufferTimeoutS(bqp, timeout);
      /* Anything except MSG_OK interrupts the operation.*/
      if (msg != MSG_OK) {
        break;
      }
    }

    /* Size of the space available in the current buffer.*/
    size = (size_t)(bqp->top - bqp->ptr);
    if (size > (n - c)) {
      size = n - c;
    }

    if (dir) memcpy(bqp->ptr, &bp[c], size); // to buffer (write)
    else     memcpy(bp, bqp->ptr, size);     // from buffer (read)
    bqp->ptr += size;
    c        += size;
    /* Has the current data buffer been finished? if so then release it.*/
    if (bqp->ptr >= bqp->top) {
      if (dir) obqPostFullBufferS(bqp, bqp->bsize - sizeof (size_t)); // Write complete (full)
      else     ibqReleaseEmptyBufferS(bqp);                           // Read complete  (empty)
    }
  } while(c < n);
  osalSysUnlock();
  return c;
}

static void bqReleaseEmptyBuffer(io_buffers_queue_t *bqp)
{
  /* Freeing a buffer slot in the queue.*/
  bqp->bcounter--;
  bqp->brdptr += bqp->bsize;
  if (bqp->brdptr >= bqp->btop) {
    bqp->brdptr = bqp->buffers;
  }
}

static void bqPrepareBuffer(io_buffers_queue_t *bqp, size_t size)
{
  /* Writing size field in the buffer.*/
  *((size_t *)(void *)bqp->bwrptr) = size;

  /* Posting the buffer in the queue.*/
  bqp->bcounter++;
  bqp->bwrptr += bqp->bsize;
  if (bqp->bwrptr >= bqp->btop) {
    bqp->bwrptr = bqp->buffers;
  }
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Initializes an input/output buffers queue object.
 *
 * @param[out] bqp      pointer to the @p io_buffers_queue_t object
 * @param[in] bp        pointer to a memory area allocated for buffers
 * @param[in] size      buffers size
 * @param[in] n         number of buffers
 * @param[in] nfy       callback called when a buffer is posted in the queue
 * @param[in] link      application defined pointer
 *
 * @init
 */
void bqObjectInit(io_buffers_queue_t *bqp, uint8_t *bp,
                   size_t size, size_t n,
                   bqnotify_t nfy, void *link) {

  osalDbgCheck((bqp != NULL) && (bp != NULL) && (size >= 2U));

  osalThreadQueueObjectInit(&bqp->waiting);
  bqp->bcounter  = 0;
  bqp->brdptr    = bp;
  bqp->bwrptr    = bp;
  bqp->btop      = bp + ((size + sizeof (size_t)) * n);
  bqp->bsize     = size + sizeof (size_t);
  bqp->bn        = n;
  bqp->buffers   = bp;
  bqp->ptr       = NULL;
  bqp->top       = NULL;
  bqp->notify    = nfy;
  bqp->link      = link;
}
/**
 * @brief   Resets an input/output buffers queue.
 * @details All the data in the input buffers queue is erased and lost, any
 *          waiting thread is resumed with status @p MSG_RESET.
 * @note    A reset operation can be used by a low level driver in order to
 *          obtain immediate attention from the high level layers.
 *
 * @param[in] bqp       pointer to the @p io_buffers_queue_t object
 *
 * @iclass
 */
void bqResetI(io_buffers_queue_t *bqp) {
  osalDbgCheckClassI();

  bqp->bcounter  = 0;
  bqp->brdptr    = bqp->buffers;
  bqp->bwrptr    = bqp->buffers;
  bqp->ptr       = NULL;
  bqp->top       = NULL;
  osalThreadDequeueAllI(&bqp->waiting, MSG_RESET);
}

/**
 * @brief   Posts a new filled buffer to the queue.
 *
 * @param[in] ibqp      pointer to the @p input_buffers_queue_t object
 * @param[in] size      used size of the buffer, cannot be zero
 *
 * @iclass
 */
void ibqPostFullBufferI(input_buffers_queue_t *ibqp, size_t size) {

  osalDbgCheckClassI();

  osalDbgCheck((size > 0U) && (size <= (ibqp->bsize - sizeof (size_t))));
  osalDbgAssert(!bqIsFullI(ibqp), "buffers queue full");
  /* Prepare buffer for input */
  bqPrepareBuffer(ibqp, size);
  /* Waking up one waiting thread, if any.*/
  bq_Restore(ibqp);
}

/**
 * @brief   Gets the next filled buffer from the queue.
 * @note    The function always acquires the same buffer if called repeatedly.
 * @post    After calling the function the fields @p ptr and @p top are set
 *          at beginning and end of the buffer data or @p NULL if the queue
 *          is empty.
 *
 * @param[in] ibqp      pointer to the @p input_buffers_queue_t object
 * @param[in] timeout   the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_IMMEDIATE immediate timeout.
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 * @return              The operation status.
 * @retval MSG_OK       if a buffer has been acquired.
 * @retval MSG_TIMEOUT  if the specified time expired.
 * @retval MSG_RESET    if the queue has been reset.
 *
 * @api
 */
msg_t ibqGetFullBufferTimeout(input_buffers_queue_t *ibqp,
                              systime_t timeout) {
  msg_t msg;

  osalSysLock();
  msg = ibqGetFullBufferTimeoutS(ibqp, timeout);
  osalSysUnlock();

  return msg;
}

/**
 * @brief   Gets the next filled buffer from the queue.
 * @note    The function always acquires the same buffer if called repeatedly.
 * @post    After calling the function the fields @p ptr and @p top are set
 *          at beginning and end of the buffer data or @p NULL if the queue
 *          is empty.
 *
 * @param[in] ibqp      pointer to the @p input_buffers_queue_t object
 * @param[in] timeout   the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_IMMEDIATE immediate timeout.
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 * @return              The operation status.
 * @retval MSG_OK       if a buffer has been acquired.
 * @retval MSG_TIMEOUT  if the specified time expired.
 * @retval MSG_RESET    if the queue has been reset.
 *
 * @sclass
 */
msg_t ibqGetFullBufferTimeoutS(input_buffers_queue_t *ibqp, systime_t timeout) {

  osalDbgCheckClassS();

  msg_t msg = waitReadyTimeout(ibqp, timeout);
  if (msg == MSG_OK) {
    osalDbgAssert(!bqIsEmptyI(ibqp), "still empty");

    /* Setting up the "current" buffer and its boundary.*/
    ibqp->ptr = ibqp->brdptr + sizeof (size_t);
    ibqp->top = ibqp->ptr + *((size_t *)(void *)ibqp->brdptr);
  }
  return msg;
}

/**
 * @brief   Releases the buffer back in the queue.
 * @note    The object callback is called after releasing the buffer.
 *
 * @param[in] ibqp      pointer to the @p input_buffers_queue_t object
 *
 * @api
 */
void ibqReleaseEmptyBuffer(input_buffers_queue_t *ibqp) {

  osalSysLock();
  ibqReleaseEmptyBufferS(ibqp);
  osalSysUnlock();
}

/**
 * @brief   Releases the buffer back in the queue.
 * @note    The object callback is called after releasing the buffer.
 *
 * @param[in] ibqp      pointer to the @p input_buffers_queue_t object
 *
 * @sclass
 */
void ibqReleaseEmptyBufferS(input_buffers_queue_t *ibqp) {

  osalDbgCheckClassS();
  osalDbgAssert(!bqIsEmptyI(ibqp), "buffers queue empty");

  /* Freeing a buffer slot in the queue.*/
  bqReleaseEmptyBuffer(ibqp);

  /* No "current" buffer.*/
  ibqp->ptr = NULL;
  /* Notifying the buffer release.*/
  bq_Notify(ibqp);
}

/**
 * @brief   Input queue read with timeout.
 * @details This function reads a byte value from an input queue. If
 *          the queue is empty then the calling thread is suspended until a
 *          new buffer arrives in the queue or a timeout occurs.
 *
 * @param[in] ibqp      pointer to the @p input_buffers_queue_t object
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
msg_t ibqGetTimeout(input_buffers_queue_t *ibqp, systime_t timeout) {
  msg_t msg;

  osalSysLock();

  /* This condition indicates that a new buffer must be acquired.*/
  if (ibqp->ptr == NULL) {
    msg = ibqGetFullBufferTimeoutS(ibqp, timeout);
    if (msg != MSG_OK) {
      osalSysUnlock();
      return msg;
    }
  }

  /* Next byte from the buffer.*/
  msg = (msg_t)*ibqp->ptr;
  ibqp->ptr++;

  /* If the current buffer has been fully read then it is returned as
     empty in the queue.*/
  if (ibqp->ptr >= ibqp->top) {
    ibqReleaseEmptyBufferS(ibqp);
  }

  osalSysUnlock();
  return msg;
}

/**
 * @brief   Input queue read with timeout.
 * @details The function reads data from an input queue into a buffer.
 *          The operation completes when the specified amount of data has been
 *          transferred or after the specified timeout or if the queue has
 *          been reset.
 *
 * @param[in] ibqp      pointer to the @p input_buffers_queue_t object
 * @param[out] bp       pointer to the data buffer
 * @param[in] n         the maximum amount of data to be transferred, the
 *                      value 0 is reserved
 * @param[in] timeout   the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_IMMEDIATE immediate timeout.
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 * @return              The number of bytes effectively transferred.
 * @retval 0            if a timeout occurred.
 *
 * @api
 */
size_t ibqReadTimeout(input_buffers_queue_t *ibqp, uint8_t *bp,
                      size_t n, systime_t timeout) {
  osalDbgCheck(n > 0U);
  return bqCopyTimeout(ibqp, false, bp, n, timeout);
}

/**
 * @brief   Gets the next empty buffer from the queue.
 * @note    The function always returns the same buffer if called repeatedly.
 *
 * @param[in] ibqp      pointer to the @p input_buffers_queue_t object
 * @return              A pointer to the next buffer to be filled.
 * @retval NULL         if the queue is full.
 *
 * @iclass
 */
uint8_t *ibqGetEmptyBufferI(input_buffers_queue_t *ibqp) {

  osalDbgCheckClassI();

  if (bqIsFullI(ibqp)) {
    return NULL;
  }

  return ibqp->bwrptr + sizeof (size_t);
}

/**
 * @brief   Gets the next filled buffer from the queue.
 * @note    The function always returns the same buffer if called repeatedly.
 *
 * @param[in] obqp      pointer to the @p output_buffers_queue_t object
 * @param[out] sizep    pointer to the filled buffer size
 * @return              A pointer to the filled buffer.
 * @retval NULL         if the queue is empty.
 *
 * @iclass
 */
uint8_t *obqGetFullBufferI(output_buffers_queue_t *obqp, size_t *sizep) {
  osalDbgCheckClassI();

  if (bqIsEmptyI(obqp)) {
    return NULL;
  }
  /* Buffer size.*/
  *sizep = *((size_t *)(void *)obqp->brdptr);
  return obqp->brdptr + sizeof (size_t);
}

/**
 * @brief   Releases the next filled buffer back in the queue.
 *
 * @param[in] obqp      pointer to the @p output_buffers_queue_t object
 *
 * @iclass
 */
void obqReleaseEmptyBufferI(output_buffers_queue_t *obqp) {

  osalDbgCheckClassI();
  osalDbgAssert(!bqIsEmptyI(obqp), "buffers queue empty");
  /* Freeing a buffer slot in the queue.*/
  bqReleaseEmptyBuffer(obqp);
  /* Waking up one waiting thread, if any.*/
  bq_Restore(obqp);
}

/**
 * @brief   Gets the next empty buffer from the queue.
 * @note    The function always acquires the same buffer if called repeatedly.
 * @post    After calling the function the fields @p ptr and @p top are set
 *          at beginning and end of the buffer data or @p NULL if the queue
 *          is empty.
 *
 * @param[in] obqp      pointer to the @p output_buffers_queue_t object
 * @param[in] timeout   the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_IMMEDIATE immediate timeout.
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 * @return              The operation status.
 * @retval MSG_OK       if a buffer has been acquired.
 * @retval MSG_TIMEOUT  if the specified time expired.
 * @retval MSG_RESET    if the queue has been reset.
 *
 * @api
 */
msg_t obqGetEmptyBufferTimeout(output_buffers_queue_t *obqp,
                                systime_t timeout) {
  msg_t msg;

  osalSysLock();
  msg = obqGetEmptyBufferTimeoutS(obqp, timeout);
  osalSysUnlock();

  return msg;
}

/**
 * @brief   Gets the next empty buffer from the queue.
 * @note    The function always acquires the same buffer if called repeatedly.
 * @post    After calling the function the fields @p ptr and @p top are set
 *          at beginning and end of the buffer data or @p NULL if the queue
 *          is empty.
 *
 * @param[in] obqp      pointer to the @p output_buffers_queue_t object
 * @param[in] timeout   the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_IMMEDIATE immediate timeout.
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 * @return              The operation status.
 * @retval MSG_OK       if a buffer has been acquired.
 * @retval MSG_TIMEOUT  if the specified time expired.
 * @retval MSG_RESET    if the queue has been reset or has been put in
 *                      suspended state.
 *
 * @sclass
 */
msg_t obqGetEmptyBufferTimeoutS(output_buffers_queue_t *obqp,
                                systime_t timeout) {

  osalDbgCheckClassS();
  msg_t msg = waitFreeTimeout(obqp, timeout);
  if (msg == MSG_OK) {
    osalDbgAssert(!bqIsFullI(obqp), "still full");
    /* Setting up the "current" buffer and its boundary.*/
    obqp->ptr = obqp->bwrptr + sizeof (size_t);
    obqp->top = obqp->bwrptr + obqp->bsize;
  }
  return msg;
}

/**
 * @brief   Posts a new filled buffer to the queue.
 * @note    The object callback is called after releasing the buffer.
 *
 * @param[in] obqp      pointer to the @p output_buffers_queue_t object
 * @param[in] size      used size of the buffer, cannot be zero
 *
 * @api
 */
void obqPostFullBuffer(output_buffers_queue_t *obqp, size_t size) {

  osalSysLock();
  obqPostFullBufferS(obqp, size);
  osalSysUnlock();
}

/**
 * @brief   Posts a new filled buffer to the queue.
 * @note    The object callback is called after releasing the buffer.
 *
 * @param[in] obqp      pointer to the @p output_buffers_queue_t object
 * @param[in] size      used size of the buffer, cannot be zero
 *
 * @sclass
 */
void obqPostFullBufferS(output_buffers_queue_t *obqp, size_t size) {

  osalDbgCheckClassS();
  osalDbgCheck((size > 0U) && (size <= (obqp->bsize - sizeof (size_t))));
  osalDbgAssert(!bqIsFullI(obqp), "buffers queue full");
  /* Prepare buffer for output */
  bqPrepareBuffer(obqp, size);
  /* No "current" buffer.*/
  obqp->ptr = NULL;
  /* Notifying the buffer release.*/
  bq_Notify(obqp);
}

/**
 * @brief   Output queue write with timeout.
 * @details This function writes a byte value to an output queue. If
 *          the queue is full then the calling thread is suspended until a
 *          new buffer is freed in the queue or a timeout occurs.
 *
 * @param[in] obqp      pointer to the @p output_buffers_queue_t object
 * @param[in] b         byte value to be transferred
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
msg_t obqPutTimeout(output_buffers_queue_t *obqp, uint8_t b,
                    systime_t timeout) {
  msg_t msg;

  osalSysLock();

  /* This condition indicates that a new buffer must be acquired.*/
  if (obqp->ptr == NULL) {
    msg = obqGetEmptyBufferTimeoutS(obqp, timeout);
    if (msg != MSG_OK) {
      osalSysUnlock();
      return msg;
    }
  }

  /* Writing the byte to the buffer.*/
  *obqp->ptr = b;
  obqp->ptr++;

  /* If the current buffer has been fully written then it is posted as
     full in the queue.*/
  if (obqp->ptr >= obqp->top) {
    obqPostFullBufferS(obqp, obqp->bsize - sizeof (size_t));
  }

  osalSysUnlock();
  return MSG_OK;
}

/**
 * @brief   Output queue write with timeout.
 * @details The function writes data from a buffer to an output queue. The
 *          operation completes when the specified amount of data has been
 *          transferred or after the specified timeout or if the queue has
 *          been reset.
 *
 * @param[in] obqp      pointer to the @p output_buffers_queue_t object
 * @param[in] bp        pointer to the data buffer
 * @param[in] n         the maximum amount of data to be transferred, the
 *                      value 0 is reserved
 * @param[in] timeout   the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_IMMEDIATE immediate timeout.
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 * @return              The number of bytes effectively transferred.
 * @retval 0            if a timeout occurred.
 *
 * @api
 */
size_t obqWriteTimeout(output_buffers_queue_t *obqp, const uint8_t *bp,
                       size_t n, systime_t timeout) {
  osalDbgCheck(n > 0U);
  return bqCopyTimeout(obqp, true, (uint8_t *)bp, n, timeout);
}

/**
 * @brief   Flushes the current, partially filled, buffer to the queue.
 * @note    The notification callback is not invoked because the function
 *          is meant to be called from ISR context. An operation status is
 *          returned instead.
 *
 * @param[in] obqp      pointer to the @p output_buffers_queue_t object
 * @return              The operation status.
 * @retval false        if no new filled buffer has been posted to the queue.
 * @retval true         if a new filled buffer has been posted to the queue.
 *
 * @iclass
 */
bool obqTryFlushI(output_buffers_queue_t *obqp) {

  osalDbgCheckClassI();

  /* If queue is empty and there is a buffer partially filled and
     it is not being written.*/
  if (bqIsEmptyI(obqp) && (obqp->ptr != NULL)) {
    size_t size = (size_t)obqp->ptr - ((size_t)obqp->bwrptr + sizeof (size_t));

    if (size > 0U) {
      bqPrepareBuffer(obqp, size);
      /* No "current" buffer.*/
      obqp->ptr = NULL;
      return true;
    }
  }
  return false;
}

/**
 * @brief   Flushes the current, partially filled, buffer to the queue.
 *
 * @param[in] obqp      pointer to the @p output_buffers_queue_t object
 *
 * @api
 */
void obqFlush(output_buffers_queue_t *obqp) {

  osalSysLock();

  /* If there is a buffer partially filled and not being written.*/
  if (obqp->ptr != NULL) {
    size_t size = ((size_t)obqp->ptr - (size_t)obqp->bwrptr) - sizeof (size_t);

    if (size > 0U) {
      obqPostFullBufferS(obqp, size);
    }
  }

  osalSysUnlock();
}
/** @} */

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
 * @file    hal_queues.h
 * @brief   I/O Queues macros and structures.
 *
 * @addtogroup HAL_QUEUES
 * @{
 */

#ifndef HAL_QUEUES_H
#define HAL_QUEUES_H

/**
 * @name    Queue functions returned status value
 * @{
 */
#define Q_OK            MSG_OK      /**< @brief Operation successful.       */
#define Q_TIMEOUT       MSG_TIMEOUT /**< @brief Timeout condition.          */
#define Q_RESET         MSG_RESET   /**< @brief Queue has been reset.       */
#define Q_EMPTY         MSG_TIMEOUT /**< @brief Queue empty.                */
#define Q_FULL          MSG_TIMEOUT /**< @brief Queue full,                 */
/** @} */

/**
 * @brief   Type of a generic I/O queue structure.
 */
typedef struct io_queue io_queue_t;

/**
 * @brief   Queue notification callback type.
 *
 * @param[in] qp        the queue pointer
 */
typedef void (*qnotify_t)(io_queue_t *qp);

/**
 * @brief   Queue size type.
 */
typedef size_t       qsize_t;

/**
 * @brief   Generic I/O queue structure.
 * @details This structure represents a generic Input or Output asymmetrical
 *          queue. The queue is asymmetrical because one end is meant to be
 *          accessed from a thread context, and thus can be blocking, the other
 *          end is accessible from interrupt handlers or from within a kernel
 *          lock zone and is non-blocking.
 */
struct io_queue {
  threads_queue_t       q_waiting;  /**< @brief Queue of waiting threads.   */
  qnotify_t             q_notify;   /**< @brief Data notification callback. */
  uint8_t               *q_buffer;  /**< @brief Pointer to the queue buffer.*/
  qsize_t               q_counter;  /**< @brief Resources counter.          */
  qsize_t               q_size;     /**< @brief Size of queue buffer.       */
  qsize_t               q_wr;       /**< @brief Write position              */
  qsize_t               q_rd;       /**< @brief Read position               */
};

/**
 * @name    Macro Functions
 * @{
 */
/**
 * @brief   Returns the queue's buffer size.
 *
 * @param[in] qp        pointer to a @p io_queue_t structure
 * @return              The buffer size.
 *
 * @xclass
 */
#define qSizeX(qp)      ((qp)->q_size)

/**
 * @brief   Queue space.
 * @details Returns the used space if used on an input queue or the empty
 *          space if used on an output queue.
 *
 * @param[in] qp        pointer to a @p io_queue_t structure
 * @return              The buffer space.
 *
 * @iclass
 */
#define qSpaceI(qp) ((qp)->q_counter)

/**
 * @extends io_queue_t
 *
 * @brief   Type of an input / output queue structure.
 * @details This structure represents a generic asymmetrical input queue.
 *          Writing to the queue is non-blocking and can be performed from
 *          interrupt handlers or from within a kernel lock zone.
 *          Reading the queue can be a blocking operation and is supposed to
 *          be performed by a system thread.
 */
typedef io_queue_t input_queue_t;
typedef io_queue_t output_queue_t;

/**
 * @name    Macro Functions
 * @{
 */
/**
 * @brief   Returns the filled space into an input queue.
 *
 * @param[in] iqp       pointer to an @p input_queue_t structure
 * @return              The number of full bytes in the queue.
 * @retval 0            if the queue is empty.
 *
 * @iclass
 */
#define qGetFullI(qp) qSpaceI(qp)

/**
 * @brief   Returns the empty space into an input queue.
 *
 * @param[in] iqp       pointer to an @p io_queue_t structure
 * @return              The number of empty bytes in the queue.
 * @retval 0            if the queue is full.
 *
 * @iclass
 */
#define qGetEmptyI(qp) (qSizeX(qp) - qSpaceI(qp))

/**
 * @brief   Evaluates to @p true if the specified input queue is empty.
 *
 * @param[in] iqp       pointer to an @p io_queue_t structure
 * @return              The queue status.
 * @retval false        if the queue is not empty.
 * @retval true         if the queue is empty.
 *
 * @iclass
 */
#define qIsEmptyI(qp) (qSpaceI(qp) == 0U)

/**
 * @brief   Evaluates to @p true if the specified input queue is full.
 *
 * @param[in] iqp       pointer to an @p input_queue_t structure
 * @return              The queue status.
 * @retval false        if the queue is not full.
 * @retval true         if the queue is full.
 *
 * @iclass
 */
#define qIsFullI(qp)  ((qp)->q_counter >= (qp)->q_size)

/**
 * @brief   Input queue read.
 * @details This function reads a byte value from an input queue. If the queue
 *          is empty then the calling thread is suspended until a byte arrives
 *          in the queue.
 *
 * @param[in] iqp       pointer to an @p input_queue_t structure
 * @return              A byte value from the queue.
 * @retval MSG_RESET    if the queue has been reset.
 *
 * @api
 */
#define iqGet(iqp) iqGetTimeout(iqp, TIME_INFINITE)
/** @} */

/**
 * @brief   Output queue write.
 * @details This function writes a byte value to an output queue. If the queue
 *          is full then the calling thread is suspended until there is space
 *          in the queue.
 *
 * @param[in] oqp       pointer to an @p output_queue_t structure
 * @param[in] b         the byte value to be written in the queue
 * @return              The operation status.
 * @retval MSG_OK       if the operation succeeded.
 * @retval MSG_RESET    if the queue has been reset.
 *
 * @api
 */
#define oqPut(oqp, b) oqPutTimeout(oqp, b, TIME_INFINITE)
 /** @} */

#ifdef __cplusplus
extern "C" {
#endif
  void qObjectInit(io_queue_t *qp, uint8_t *bp, qsize_t size, qnotify_t infy);

  void qResetI(io_queue_t *qp);
  msg_t qPutI(io_queue_t *qp, uint8_t b);
  msg_t qGetI(io_queue_t *qp);

  msg_t   iqGetTimeout(input_queue_t *iqp, systime_t timeout);
  qsize_t iqReadTimeout(input_queue_t *iqp, uint8_t *bp, qsize_t n, systime_t timeout);

  msg_t   oqPutTimeout(output_queue_t *oqp, uint8_t b, systime_t timeout);
  qsize_t oqWriteTimeout(output_queue_t *oqp, const uint8_t *bp, qsize_t n, systime_t timeout);
#ifdef __cplusplus
}
#endif

#endif /* HAL_QUEUES_H */

/** @} */

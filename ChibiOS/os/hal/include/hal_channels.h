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
 * @file    hal_channels.h
 * @brief   I/O channels access.
 * @details This header defines an abstract interface useful to access generic
 *          I/O serial devices in a standardized way.
 *
 * @addtogroup IO_CHANNEL
 * @details This module defines an abstract interface for I/O channels by
 *          extending the @p BaseSequentialStream interface.<br>
 *          Note that no code is present, I/O channels are just abstract
 *          interface like structures, you should look at the systems as
 *          to a set of abstract C++ classes (even if written in C).
 *          Specific device drivers can use/extend the interface and
 *          implement them.<br>
 *          This system has the advantage to make the access to channels
 *          independent from the implementation logic.
 * @{
 */

#ifndef HAL_CHANNELS_H
#define HAL_CHANNELS_H

/**
 * @brief   @p BaseChannel specific methods.
 */
#define _base_channel_methods                                               \
  _base_sequential_stream_methods                                           \
  /* Channel put method with timeout specification.*/                       \
  msg_t (*putt)(void *instance, uint8_t b, systime_t time);                 \
  /* Channel get method with timeout specification.*/                       \
  msg_t (*gett)(void *instance, systime_t time);                            \
  /* Channel write method with timeout specification.*/                     \
  size_t (*writet)(void *instance, const uint8_t *bp,                       \
                   size_t n, systime_t time);                               \
  /* Channel read method with timeout specification.*/                      \
  size_t (*readt)(void *instance, uint8_t *bp, size_t n, systime_t time);

/**
 * @brief   @p BaseChannel specific data.
 * @note    It is empty because @p BaseChannel is only an interface without
 *          implementation.
 */
#define _base_channel_data                                                  \
  _base_sequential_stream_data

/**
 * @extends BaseSequentialStreamVMT
 *
 * @brief   @p BaseChannel virtual methods table.
 */
struct BaseChannelVMT {
  _base_channel_methods
};

/**
 * @extends BaseSequentialStream
 *
 * @brief   Base channel class.
 * @details This class represents a generic, byte-wide, I/O channel. This class
 *          introduces generic I/O primitives with timeout specification.
 */
typedef struct {
  /** @brief Virtual Methods Table.*/
  const struct BaseChannelVMT *vmt;
  _base_channel_data
} BaseChannel;

/**
 * @name    Macro Functions (BaseChannel)
 * @{
 */
/**
 * @brief   Channel blocking byte write with timeout.
 * @details This function writes a byte value to a channel. If the channel
 *          is not ready to accept data then the calling thread is suspended.
 *
 * @param[in] ip        pointer to a @p BaseChannel or derived class
 * @param[in] b         the byte value to be written to the channel
 * @param[in] time      the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_IMMEDIATE immediate timeout.
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 * @return              The operation status.
 * @retval STM_OK       if the operation succeeded.
 * @retval STM_TIMEOUT  if the specified time expired.
 * @retval STM_RESET    if the channel associated queue (if any) was reset.
 *
 * @api
 */
#define chnPutTimeout(ip, b, time) ((ip)->vmt->putt(ip, b, time))

/**
 * @brief   Channel blocking byte read with timeout.
 * @details This function reads a byte value from a channel. If the data
 *          is not available then the calling thread is suspended.
 *
 * @param[in] ip        pointer to a @p BaseChannel or derived class
 * @param[in] time      the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_IMMEDIATE immediate timeout.
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 * @return              A byte value from the queue.
 * @retval STM_TIMEOUT  if the specified time expired.
 * @retval STM_RESET    if the channel associated queue (if any) has been
 *                      reset.
 *
 * @api
 */
#define chnGetTimeout(ip, time) ((ip)->vmt->gett(ip, time))

/**
 * @brief   Channel blocking write.
 * @details The function writes data from a buffer to a channel. If the channel
 *          is not ready to accept data then the calling thread is suspended.
 *
 * @param[in] ip        pointer to a @p BaseChannel or derived class
 * @param[out] bp       pointer to the data buffer
 * @param[in] n         the maximum amount of data to be transferred
 *
 * @return              The number of bytes transferred.
 *
 * @api
 */
#define chnWrite(ip, bp, n) streamWrite(ip, bp, n)

/**
 * @brief   Channel blocking write with timeout.
 * @details The function writes data from a buffer to a channel. If the channel
 *          is not ready to accept data then the calling thread is suspended.
 *
 * @param[in] ip        pointer to a @p BaseChannel or derived class
 * @param[out] bp       pointer to the data buffer
 * @param[in] n         the maximum amount of data to be transferred
 * @param[in] time      the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_IMMEDIATE immediate timeout.
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 * @return              The number of bytes transferred.
 *
 * @api
 */
#define chnWriteTimeout(ip, bp, n, time) ((ip)->vmt->writet(ip, bp, n, time))

/**
 * @brief   Channel blocking read.
 * @details The function reads data from a channel into a buffer. If the data
 *          is not available then the calling thread is suspended.
 *
 * @param[in] ip        pointer to a @p BaseChannel or derived class
 * @param[in] bp        pointer to the data buffer
 * @param[in] n         the maximum amount of data to be transferred
 *
 * @return              The number of bytes transferred.
 *
 * @api
 */
#define chnRead(ip, bp, n) streamRead(ip, bp, n)

/**
 * @brief   Channel blocking read with timeout.
 * @details The function reads data from a channel into a buffer. If the data
 *          is not available then the calling thread is suspended.
 *
 * @param[in] ip        pointer to a @p BaseChannel or derived class
 * @param[in] bp        pointer to the data buffer
 * @param[in] n         the maximum amount of data to be transferred
 * @param[in] time      the number of ticks before the operation timeouts,
 *                      the following special values are allowed:
 *                      - @a TIME_IMMEDIATE immediate timeout.
 *                      - @a TIME_INFINITE no timeout.
 *                      .
 * @return              The number of bytes transferred.
 *
 * @api
 */
#define chnReadTimeout(ip, bp, n, time) ((ip)->vmt->readt(ip, bp, n, time))
/** @} */

/**
 * @name    I/O status flags added to the event listener
 * @{
 */
/** @brief No pending conditions.*/
#define CHN_NO_ERROR            (eventflags_t)0
/** @brief Connection happened.*/
#define CHN_CONNECTED           (eventflags_t)1
/** @brief Disconnection happened.*/
#define CHN_DISCONNECTED        (eventflags_t)2
/** @brief Data available in the input queue.*/
#define CHN_INPUT_AVAILABLE     (eventflags_t)4
/** @brief Output queue empty.*/
#define CHN_OUTPUT_EMPTY        (eventflags_t)8
/** @brief Transmission end.*/
#define CHN_TRANSMISSION_END    (eventflags_t)16
/** @} */

/**
 * @brief   @p BaseAsynchronousChannel specific methods.
 */
#define _base_asynchronous_channel_methods                                  \
  _base_channel_methods                                                     \

/**
 * @brief   @p BaseAsynchronousChannel specific data.
 */
#define _base_asynchronous_channel_data                                     \
  _base_channel_data                                                        \
  /* I/O condition event source.*/                                          \
  event_source_t        event;

/**
 * @extends BaseChannelVMT
 *
 * @brief   @p BaseAsynchronousChannel virtual methods table.
 */
struct BaseAsynchronousChannelVMT {
  _base_asynchronous_channel_methods
};

/**
 * @extends BaseChannel
 *
 * @brief   Base asynchronous channel class.
 * @details This class extends @p BaseChannel by adding event sources fields
 *          for asynchronous I/O for use in an event-driven environment.
 */
typedef struct {
  /** @brief Virtual Methods Table.*/
  const struct BaseAsynchronousChannelVMT *vmt;
  _base_asynchronous_channel_data
} BaseAsynchronousChannel;

/**
 * @name    Macro Functions (BaseAsynchronousChannel)
 * @{
 */
/**
 * @brief   Returns the I/O condition event source.
 * @details The event source is broadcasted when an I/O condition happens.
 *
 * @param[in] ip        pointer to a @p BaseAsynchronousChannel or derived
 *                      class
 * @return              A pointer to an @p EventSource object.
 *
 * @api
 */
#define chnGetEventSource(ip) (&((ip)->event))

/**
 * @brief   Adds status flags to the listeners's flags mask.
 * @details This function is usually called from the I/O ISRs in order to
 *          notify I/O conditions such as data events, errors, signal
 *          changes etc.
 *
 * @param[in] ip        pointer to a @p BaseAsynchronousChannel or derived
 *                      class
 * @param[in] flags     condition flags to be added to the listener flags mask
 *
 * @iclass
 */
#define chnAddFlagsI(ip, flags) {                                           \
  osalEventBroadcastFlagsI(&(ip)->event, flags);                            \
}
/** @} */

#endif /* HAL_CHANNELS_H */

/** @} */

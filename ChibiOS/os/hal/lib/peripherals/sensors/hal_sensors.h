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
 * @file    hal_sensors.h
 * @brief   Generic sensors interface header.
 *
 * @addtogroup HAL_SENSORS
 * @{
 */

#ifndef _HAL_SENSORS_H_
#define _HAL_SENSORS_H_

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   BaseSensor specific methods.
 */
#define _base_sensor_methods_alone                                          \
  /* Get number of axes.*/                                                  \
  size_t (*get_axes_number)(void *instance);                                \
  /* Reads the sensor raw data.*/                                           \
  msg_t (*read_raw)(void *instance, int32_t axes[]);                        \
  /* Reads the sensor returning normalized data.*/                          \
  msg_t (*read_cooked)(void *instance, float axes[]);

/**
 * @brief   BaseSensor specific methods with inherited ones.
 */
#define _base_sensor_methods                                                \
  _base_sensor_methods_alone

/**
 * @brief   @p BaseSensor virtual methods table.
 */
struct BaseSensorVMT {
  _base_sensor_methods
};

/**
 * @brief   @p BaseSensor specific data.
 * @note    It is empty because @p BaseSensor is only an interface
 *          without implementation.
 */
#define _base_sensor_data

/**
 * @brief   Base stream class.
 * @details This class represents a generic blocking unbuffered sequential
 *          data stream.
 */
typedef struct {
  /** @brief Virtual Methods Table.*/
  const struct BaseSensorVMT *vmt_basesensor;
  _base_sensor_data
} BaseSensor;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/**
 * @name    Macro Functions (BaseSensor)
 * @{
 */
/**
 * @brief   Sensors get axes number.
 *
 * @param[in] ip        pointer to a @p BaseSensor or derived class.
 * @return              The number of axes of the BaseSensor
 *
 * @api
 */
#define sensorGetAxesNumber(ip) (ip)->vmt_basesensor->get_axes_number(ip)

/**
 * @brief   Sensors read raw data.
 *
 * @param[in] ip        pointer to a @p BaseSensor or derived class.
 * @param[in] dp        pointer to a data array.
 * 
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    if one or more errors occurred.
 *
 * @api
 */
#define sensorReadRaw(ip, dp) (ip)->vmt_basesensor->read_raw(ip, dp)

/**
 * @brief   Sensors read cooked data.
 *
 * @param[in] ip        pointer to a @p BaseSensor or derived class.
 * @param[in] dp        pointer to a data array.
 * 
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    if one or more errors occurred.
 *
 * @api
 */
#define sensorReadCooked(ip, dp) (ip)->vmt_basesensor->read_cooked(ip, dp)
/** @} */

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* _HAL_SENSORS_H_ */

/** @} */

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
 * @file    hal.h
 * @brief   HAL subsystem header.
 *
 * @addtogroup HAL
 * @{
 */

#ifndef HAL_H
#define HAL_H

#include "osal.h"
#include "board.h"
#include "halconf.h"

#include "hal_lld.h"

/* Abstract interfaces.*/
#include "hal_streams.h"
#include "hal_channels.h"
#include "hal_files.h"
#include "hal_ioblock.h"
#include "hal_mmcsd.h"

/* Shared headers.*/
#include "hal_buffers.h"
#include "hal_queues.h"

/* Normal drivers.*/
#include "hal_pal.h"
#include "hal_adc.h"
#include "hal_can.h"
#include "hal_dac.h"
#include "hal_ext.h"
#include "hal_gpt.h"
#include "hal_i2c.h"
#include "hal_i2s.h"
#include "hal_icu.h"
#include "hal_mac.h"
#include "hal_pwm.h"
#include "hal_rtc.h"
#include "hal_serial.h"
#include "hal_sdc.h"
#include "hal_spi.h"
#include "hal_uart.h"
#include "hal_usb.h"
#include "hal_wdg.h"

/*
 *  The ST driver is a special case, it is only included if the OSAL is
 *  configured to require it.
 */
#if OSAL_ST_MODE != OSAL_ST_MODE_NONE
#include "hal_st.h"
#endif

/* Complex drivers.*/
#include "hal_mmc_spi.h"
#include "hal_serial_usb.h"

/* Community drivers.*/
#if defined(HAL_USE_COMMUNITY) || defined(__DOXYGEN__)
#if (HAL_USE_COMMUNITY == TRUE) || defined(__DOXYGEN__)
#include "hal_community.h"
#endif
#endif

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/**
 * @brief   ChibiOS/HAL identification macro.
 */
#define _CHIBIOS_HAL_

/**
 * @brief   Stable release flag.
 */
#define CH_HAL_STABLE           0

/**
 * @name    ChibiOS/HAL version identification
 * @{
 */
/**
 * @brief   HAL version string.
 */
#define HAL_VERSION             "5.0.0"

/**
 * @brief   HAL version major number.
 */
#define CH_HAL_MAJOR            5

/**
 * @brief   HAL version minor number.
 */
#define CH_HAL_MINOR            0

/**
 * @brief   HAL version patch number.
 */
#define CH_HAL_PATCH            0
/** @} */

/**
 * @name    Return codes
 * @{
 */
#define HAL_SUCCESS             false
#define HAL_FAILED              true
/** @} */

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void halInit(void);
#ifdef __cplusplus
}
#endif

#endif /* HAL_H */

/** @} */

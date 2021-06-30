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
 * @file    TIMv1/hal_st_lld.h
 * @brief   ST Driver subsystem low level driver header.
 * @details This header is designed to be include-able without having to
 *          include other files from the HAL.
 *
 * @addtogroup ST
 * @{
 */

#ifndef HAL_ST_LLD_H
#define HAL_ST_LLD_H

#include "mcuconf.h"
#include "stm32_registry.h"
#include "stm32_tim.h"

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    Configuration options
 * @{
 */
/**
 * @brief   SysTick timer IRQ priority.
 */
#if !defined(STM32_ST_IRQ_PRIORITY) || defined(__DOXYGEN__)
#define STM32_ST_IRQ_PRIORITY               8
#endif

/**
 * @brief   TIMx unit (by number) to be used for free running operations.
 * @note    You must select a 32 bits timer if a 32 bits @p systick_t type
 *          is required.
 * @note    Timers 2, 3, 4 and 5 are supported.
 */
#if !defined(STM32_ST_USE_TIMER) || defined(__DOXYGEN__)
#define STM32_ST_USE_TIMER                  2
#endif
/** @} */

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if STM32_ST_USE_TIMER == 2
#if !STM32_HAS_TIM2
#error "TIM2 not present"
#endif
#define STM32_ST_TIM                              STM32_TIM2

#elif STM32_ST_USE_TIMER == 3
#if !STM32_HAS_TIM3
#error "TIM3 not present"
#endif
#define STM32_ST_TIM                              STM32_TIM3

#elif STM32_ST_USE_TIMER == 4
#if !STM32_HAS_TIM4
#error "TIM4 not present"
#endif
#define STM32_ST_TIM                              STM32_TIM4

#elif STM32_ST_USE_TIMER == 5
#if !STM32_HAS_TIM5
#error "TIM5 not present"
#endif
#define STM32_ST_TIM                              STM32_TIM5

#elif STM32_ST_USE_TIMER == 21
#if !STM32_HAS_TIM21
#error "TIM21 not present"
#endif
#define STM32_ST_TIM                              STM32_TIM21

#elif STM32_ST_USE_TIMER == 22
#if !STM32_HAS_TIM22
#error "TIM22 not present"
#endif
#define STM32_ST_TIM                              STM32_TIM22

#else
#error "STM32_ST_USE_TIMER specifies an unsupported timer"
#endif

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
  void st_lld_init(void);
#ifdef __cplusplus
}
#endif

/*===========================================================================*/
/* Driver inline functions.                                                  */
/*===========================================================================*/

/**
 * @brief   Returns the time counter value.
 *
 * @return              The counter value.
 *
 * @notapi
 */
static inline systime_t st_lld_get_counter(void) {

  return (systime_t)STM32_ST_TIM->CNT;
}

/**
 * @brief   Starts the alarm.
 * @note    Makes sure that no spurious alarms are triggered after
 *          this call.
 *
 * @param[in] time      the time to be set for the first alarm
 *
 * @notapi
 */
static inline void st_lld_start_alarm(systime_t time) {

  STM32_ST_TIM->CCR[0] = (uint32_t)time;
  STM32_ST_TIM->SR     = 0;
  STM32_ST_TIM->DIER   = STM32_TIM_DIER_CC1IE;
}

/**
 * @brief   Stops the alarm interrupt.
 *
 * @notapi
 */
static inline void st_lld_stop_alarm(void) {

  STM32_ST_TIM->DIER = 0;
}

/**
 * @brief   Sets the alarm time.
 *
 * @param[in] time      the time to be set for the next alarm
 *
 * @notapi
 */
static inline void st_lld_set_alarm(systime_t time) {

  STM32_ST_TIM->CCR[0] = (uint32_t)time;
}

/**
 * @brief   Returns the current alarm time.
 *
 * @return              The currently set alarm time.
 *
 * @notapi
 */
static inline systime_t st_lld_get_alarm(void) {

  return (systime_t)STM32_ST_TIM->CCR[0];
}

/**
 * @brief   Determines if the alarm is active.
 *
 * @return              The alarm status.
 * @retval false        if the alarm is not active.
 * @retval true         is the alarm is active
 *
 * @notapi
 */
static inline bool st_lld_is_alarm_active(void) {

  return (bool)((STM32_ST_TIM->DIER & STM32_TIM_DIER_CC1IE) != 0);
}

#endif /* HAL_ST_LLD_H */

/** @} */

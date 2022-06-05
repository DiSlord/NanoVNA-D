/*
 * Copyright (c) 2019-2020, Dmitry (DiSlord) dislordlive@gmail.com
 * Based on TAKAHASHI Tomohiro (TTRFTECH) edy555@gmail.com
 * All rights reserved.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * The software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#include "ch.h"
#include "hal.h"
#include "nanovna.h"

#ifndef VNA_ADC_H
#define VNA_ADC_H
#if HAL_USE_ADC == TRUE
#error "Error VNA use self ADC lib, define HAL_USE_ADC = FALSE in halconf.h"
#endif
// Measure vbat every 5 second
#define VBAT_MEASURE_INTERVAL   S2ST(5)
#ifdef NANOVNA_F303
#include "NANOVNA_STM32_F303/adc.c"
#else
#include "NANOVNA_STM32_F072/adc.c"
#endif
#endif

#ifndef VNA_I2C_H
#define VNA_I2C_H
// Compact STM32 RTC time library
#if HAL_USE_I2C == TRUE
#error "Error VNA use self I2C lib, define HAL_USE_I2C = FALSE in halconf.h"
#endif
#ifdef NANOVNA_F303
#include "NANOVNA_STM32_F303/i2c.c"
#else
#include "NANOVNA_STM32_F072/i2c.c"
#endif
#endif

#ifndef VNA_RTC_H
#define VNA_RTC_H
#ifdef __USE_RTC__
// Compact STM32 RTC time library
#if HAL_USE_RTC == TRUE
#error "Error VNA use self RTC lib, define HAL_USE_RTC = FALSE in halconf.h"
#endif
#ifdef NANOVNA_F303
#include "NANOVNA_STM32_F303/rtc.c"
#else
#include "NANOVNA_STM32_F072/rtc.c"
#endif
#endif
#endif

#ifndef VNA_DAC_H
#define VNA_DAC_H
// Check DAC enabled in ChibiOS
#if HAL_USE_DAC == TRUE
#error "Need disable HAL_USE_DAC in halconf.h for use VNA_DAC"
#endif

#ifdef NANOVNA_F303
#include "NANOVNA_STM32_F303/dac.c"
#else
#include "NANOVNA_STM32_F072/dac.c"
#endif
#endif

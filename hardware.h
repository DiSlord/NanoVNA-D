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

/*
 * adc.c
 */
#if defined(NANOVNA_F303)
#define ADC_TOUCH_X  3
#define ADC_TOUCH_Y  4
#else
#define ADC_TOUCH_X  ADC_CHSELR_CHSEL6
#define ADC_TOUCH_Y  ADC_CHSELR_CHSEL7
#endif

void adc_init(void);
uint16_t adc_single_read(uint32_t chsel);
void adc_start_analog_watchdog(void);
void adc_stop_analog_watchdog(void);
int16_t adc_vbat_read(void);

/*
 * i2c.c
 */
void i2c_start(void);
void i2c_set_timings(uint32_t timings);
bool i2c_transfer(uint8_t addr, const uint8_t *w, size_t wn);
//bool i2c_transfer(uint8_t addr, const uint8_t *w, size_t wn, uint8_t *r, size_t rn);

/*
 * rtc.c
 */
#ifdef __USE_RTC__
#define RTC_START_YEAR          2000

#define RTC_DR_YEAR(dr)         (((dr)>>16)&0xFF)
#define RTC_DR_MONTH(dr)        (((dr)>> 8)&0xFF)
#define RTC_DR_DAY(dr)          (((dr)>> 0)&0xFF)

#define RTC_TR_HOUR(dr)         (((tr)>>16)&0xFF)
#define RTC_TR_MIN(dr)          (((tr)>> 8)&0xFF)
#define RTC_TR_SEC(dr)          (((tr)>> 0)&0xFF)

// Init RTC
void rtc_init(void);
// Then read time and date TR should read first, after DR !!!
// Get RTC time as bcd structure in 0x00HHMMSS
#define rtc_get_tr_bcd() (RTC->TR & 0x007F7F7F)
// Get RTC date as bcd structure in 0x00YYMMDD (remove day of week information!!!!)
#define rtc_get_dr_bcd() (RTC->DR & 0x00FF1F3F)
// read TR as 0x00HHMMSS in bin (TR should be read first for sync)
uint32_t rtc_get_tr_bin(void);
// read DR as 0x00YYMMDD in bin (DR should be read second)
uint32_t rtc_get_dr_bin(void);
// Read time in FAT filesystem format
uint32_t rtc_get_FAT(void);
// Write date and time (need in bcd format!!!)
void rtc_set_time(uint32_t dr, uint32_t tr);
#endif

/*
 * dac.c
 */
void dac_init(void);
void dac_setvalue_ch1(uint16_t v);
void dac_setvalue_ch2(uint16_t v);

/*
 * flash.c
 */
#if defined(NANOVNA_F303)
// For STM32F303xC CPU setting
#define FLASH_START_ADDRESS   0x08000000
#define FLASH_TOTAL_SIZE     (256*1024)

#define FLASH_PAGESIZE 0x800

#define SAVEAREA_MAX 7

// Depend from config_t size, should be aligned by FLASH_PAGESIZE
#define SAVE_CONFIG_SIZE        0x00000800
// Depend from properties_t size, should be aligned by FLASH_PAGESIZE
#define SAVE_PROP_CONFIG_SIZE   0x00004000
#else
// For STM32F072xB CPU setting
#define FLASH_START_ADDRESS   0x08000000
#define FLASH_TOTAL_SIZE     (128*1024)

#define FLASH_PAGESIZE 0x800

#define SAVEAREA_MAX 5

// Depend from config_t size, should be aligned by FLASH_PAGESIZE
#define SAVE_CONFIG_SIZE        0x00000800
// Depend from properties_t size, should be aligned by FLASH_PAGESIZE
#define SAVE_PROP_CONFIG_SIZE   0x00001800
#endif

// Save config_t and properties_t flash area (see flash7 from *.ld settings)
#define SAVE_FULL_AREA_SIZE     (SAVE_CONFIG_SIZE + SAVEAREA_MAX * SAVE_PROP_CONFIG_SIZE)
// Save setting at end of CPU flash area
// Config at end minus full size
#define SAVE_CONFIG_ADDR        (FLASH_START_ADDRESS + FLASH_TOTAL_SIZE - SAVE_FULL_AREA_SIZE)
// Properties save area follow after config
#define SAVE_PROP_CONFIG_ADDR   (SAVE_CONFIG_ADDR + SAVE_CONFIG_SIZE)

void flash_erase_pages(uint32_t page_address, uint32_t size);
void flash_program_half_word_buffer(uint16_t* dst, uint16_t *data, uint16_t size);
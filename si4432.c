/* Copyright (c) 2020, Dmitry Slepynin (DiSlord) dislordlive@gmail.com
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
#include "si4432.h"
#include "spi.h"

// Define for use hardware SPI mode
#define USE_HARDWARE_SPI_MODE

// 10MHz clock
#define SI4432_10MHZ 10000000U
// !!!! FROM ili9341.c for disable it !!!! 
#define LCD_CS_HIGH    palSetPad(GPIOB, GPIOB_LCD_CS)
#define SI_CS_LOW      palClearPad(GPIOA, GPIOA_SI_SEL)
#define SI_CS_HIGH     palSetPad(GPIOA, GPIOA_SI_SEL)

// Hardware or software SPI use
#ifdef USE_HARDWARE_SPI_MODE
#define SI4432_SPI         SPI1
#define SI4432_SPI_SPEED   SPI_BR_DIV8

static uint32_t old_spi_settings;

#else

#define SPI_CLK_HIGH   palSetPad(GPIOB, GPIOB_SPI_SCLK)
#define SPI_CLK_LOW    palClearPad(GPIOB, GPIOB_SPI_SCLK)

#define SPI_SDI_HIGH   palSetPad(GPIOB, GPIOB_SPI_MOSI)
#define SPI_SDI_LOW    palClearPad(GPIOB, GPIOB_SPI_MOSI)
#define SPI_RESET      palClearPort(GPIOB, (1<<GPIOB_SPI_SCLK)|(1<<GPIOB_SPI_MOSI))

#define SPI_SDO       ((palReadPort(GPIOB)>>GPIOB_SPI_MISO)&1)
#define SPI_portSDO   (palReadPort(GPIOB)&(1<<GPIOB_SPI_MISO))

static uint32_t old_port_moder;
static uint32_t new_port_moder;
#endif

void SI4432_Select(void){
  LCD_CS_HIGH;
#ifdef USE_HARDWARE_SPI_MODE
  old_spi_settings = SI4432_SPI->CR1;
  SPI_BR_SET(SI4432_SPI, SI4432_SPI_SPEED);
#else
  // Init legs mode for software bitbang
  old_port_moder = GPIOB->MODER;
  new_port_moder = old_port_moder & ~(PIN_MODE_ANALOG(GPIOB_SPI_SCLK)|PIN_MODE_ANALOG(GPIOB_SPI_MISO)|PIN_MODE_ANALOG(GPIOB_SPI_MOSI));
  new_port_moder|= PIN_MODE_OUTPUT(GPIOB_SPI_SCLK)|PIN_MODE_INPUT(GPIOB_SPI_MISO)|PIN_MODE_OUTPUT(GPIOB_SPI_MOSI);
  GPIOB->MODER = new_port_moder;
  // Pull down SPI
  SPI_SDI_LOW;
  SPI_CLK_LOW;
#endif
}

void SI4432_Deselect(void){
  SI_CS_HIGH;
#ifdef USE_HARDWARE_SPI_MODE
  SI4432_SPI->CR1 = old_spi_settings;
#else
  // Restore hardware SPI
  GPIOB->MODER = old_port_moder;
#endif
}

static void SI4432_shiftOut(uint8_t val)
{
#ifdef USE_HARDWARE_SPI_MODE
  SPI_WRITE_8BIT(SI4432_SPI, val);
  while (SPI_IS_BUSY(SI4432_SPI)) // drop rx and wait tx
    (void)SPI_READ_8BIT(SI4432_SPI);
#else
  uint8_t i = 0;
  do {
    if (val & 0x80)
      SPI_SDI_HIGH;
    SPI_CLK_HIGH;
    SPI_RESET;
    val<<=1;
  }while((++i) & 0x07);
#endif
}

static uint8_t SI4432_shiftIn(void)
{
#ifdef USE_HARDWARE_SPI_MODE
  SPI_WRITE_8BIT(SI4432_SPI, 0xFF);
  while (SPI_RX_IS_EMPTY(SI4432_SPI)); //wait rx data in buffer
  return SPI_READ_8BIT(SI4432_SPI);
#else
  uint32_t value = 0;
  uint8_t i = 0;
  do {
    value<<=1;
    SPI_CLK_HIGH;
    value|=SPI_portSDO;
    SPI_CLK_LOW;
  }while((++i) & 0x07);
  return value>>GPIOB_SPI_MISO;
#endif
}

void SI4432_Reset(void)
{
  int count = 0;
  SI4432_Read_Byte (SI4432_INT_STATUS1);    // Clear pending interrupts
  SI4432_Read_Byte (SI4432_INT_STATUS2);
  // always perform a system reset (don't send 0x87)
  SI4432_Write_Byte(SI4432_STATE, 0x80);
  chThdSleepMilliseconds(10);
  // wait for chiprdy bit
  while (count++ < 100 && ( SI4432_Read_Byte (SI4432_INT_STATUS2) & 0x02 ) == 0) {
    chThdSleepMilliseconds(10);
  }
}

void SI4432_Init(void){
  // Store old port settings for software SPI mode
#ifndef USE_HARDWARE_SPI_MODE

#endif

  SI4432_Select();
  SI4432_Reset();
  SI4432_Write_Byte(SI4432_STATE, 0x80);
  chThdSleepMilliseconds(10);

  SI4432_Write_Byte(SI4432_AGC_OVERRIDE, 0x60); //AGC override according to WBS3
  SI4432_Write_Byte(SI4432_INT_ENABLE1, 0x0);
  SI4432_Write_Byte(SI4432_INT_ENABLE2, 0x0);
  // Enable receiver chain
//  SI4432_Write_Byte(SI4432_STATE, 0x05);
  // Clock Recovery Gearshift Value
  SI4432_Write_Byte(SI4432_CLOCK_RECOVERY_GEARSHIFT, 0x00);
  // IF Filter Bandwidth
//  // Register 0x75 Frequency Band Select
//  byte sbsel = 1 ;  // recommended setting
//  byte hbsel = 0 ;  // low bands
//  byte fb = 19 ;    // 430�439.9 MHz
//  byte FBS = (sbsel << 6 ) | (hbsel << 5 ) | fb ;
//  SI4432_Write_Byte(SI4432_FREQBAND, FBS) ;
//  SI4432_Write_Byte(SI4432_FREQBAND, 0x46) ;
  // Register 0x76 Nominal Carrier Frequency
  // WE USE 433.92 MHz
  // Si443x-Register-Settings_RevB1.xls
//  SI4432_Write_Byte(SI4432_FREQCARRIER_H, 0x62) ;
//  SI4432_Write_Byte(SI4432_FREQCARRIER_H, 0x00) ;
  // Register 0x77 Nominal Carrier Frequency
//  SI4432_Write_Byte(SI4432_FREQCARRIER_L, 0x00) ;
  // RX MODEM SETTINGS
//  SI4432_Write_3_Byte(SI4432_IF_FILTER_BW, 0x81, 0x3C, 0x02) ;    // <----------
//  SI4432_Write_Byte(SI4432_IF_FILTER_BW, 0x81) ;    // <----------
  SI4432_Write_Byte(SI4432_AFC_LOOP_GEARSHIFT_OVERRIDE, 0x00) ;
//  SI4432_Write_Byte(SI4432_AFC_TIMING_CONTROL, 0x02) ;    // <----------
  SI4432_Write_Byte(SI4432_CLOCK_RECOVERY_GEARSHIFT, 0x03) ;
//  SI4432_Write_Byte(SI4432_CLOCK_RECOVERY_OVERSAMPLING, 0x78) ;    // <----------
//  SI4432_Write_3_Byte(SI4432_CLOCK_RECOVERY_OFFSET2, 0x01, 0x11, 0x11) ;    // <----------
  SI4432_Write_Byte(SI4432_CLOCK_RECOVERY_OFFSET2, 0x01) ;
  SI4432_Write_Byte(SI4432_CLOCK_RECOVERY_OFFSET1, 0x11) ;
  SI4432_Write_Byte(SI4432_CLOCK_RECOVERY_OFFSET0, 0x11) ;
  SI4432_Write_Byte(SI4432_CLOCK_RECOVERY_TIMING_GAIN1, 0x01) ;
  SI4432_Write_Byte(SI4432_CLOCK_RECOVERY_TIMING_GAIN0, 0x13) ;
  SI4432_Write_Byte(SI4432_AFC_LIMITER, 0xFF) ;

//  SI4432_Write_3_Byte(0x2C, 0x28, 0x0c, 0x28) ;    // <----------
//  SI4432_Write_Byte(Si4432_OOK_COUNTER_VALUE_1, 0x28) ;
//  SI4432_Write_Byte(Si4432_OOK_COUNTER_VALUE_2, 0x0C) ;
//  SI4432_Write_Byte(Si4432_SLICER_PEAK_HOLD, 0x28) ;

  SI4432_Write_Byte(SI4432_DATAACCESS_CONTROL, 0x61); // Disable all packet handling
  SI4432_Write_Byte(SI4432_AGC_OVERRIDE, 0x60); // AGC, no LNA, fast gain increment

  // Switch off si4432
  SI4432_switch(false);
  SI4432_Deselect();
}

void SI4432_Write_Byte(uint8_t ADR, uint8_t DATA)
{
  SI_CS_LOW;
  ADR |= 0x80 ; // RW = 1
  SI4432_shiftOut( ADR );
  SI4432_shiftOut( DATA );
  SI_CS_HIGH;
}

uint8_t SI4432_Read_Byte( uint8_t ADR )
{
  uint8_t DATA ;
  SI_CS_LOW;
  SI4432_shiftOut( ADR );
  DATA = SI4432_shiftIn();
  SI_CS_HIGH;
  return DATA;
}

void SI4432_switch_on(void) {
  SI4432_Write_Byte(SI4432_GPIO0_CONF, 0x1f); // GPIO0 to GND
  SI4432_Write_Byte(SI4432_GPIO1_CONF, 0x1d); // GPIO1 to VDD
  int count = 0;
  if (( SI4432_Read_Byte(SI4432_DEV_STATUS) & 0x03 ) == 2)
    return; // Already in transmit mode
  chThdSleepMilliseconds(3);
  SI4432_Write_Byte(SI4432_STATE, 0x02);
  chThdSleepMilliseconds(3);
  SI4432_Write_Byte(SI4432_STATE, 0x0b);
  chThdSleepMilliseconds(10);
  while (count++ < 100 && ( SI4432_Read_Byte(SI4432_DEV_STATUS) & 0x03 ) != 2) {
    chThdSleepMilliseconds(10);
  }
}

void SI4432_switch_off(void) {
  SI4432_Write_Byte(SI4432_GPIO0_CONF, 0x1d); // GPIO0 to VDD
  SI4432_Write_Byte(SI4432_GPIO1_CONF, 0x1f); // GPIO1 to GND
  int count = 0;
  if (( SI4432_Read_Byte (SI4432_DEV_STATUS) & 0x03 ) == 1)
    return; // Already in receive mode
  chThdSleepMilliseconds(3);
  SI4432_Write_Byte(SI4432_STATE, 0x02);
  chThdSleepMilliseconds(3);
  SI4432_Write_Byte(SI4432_STATE, 0x07);
  chThdSleepMilliseconds(10);
  while (count++ < 100 && ( SI4432_Read_Byte(SI4432_DEV_STATUS) & 0x03 ) != 1) {
    chThdSleepMilliseconds(5);
  }
}

static bool si_enabled = false;
void SI4432_switch(bool en){
  if (si_enabled == en) return;
  SI4432_Select();
  if (en)
    SI4432_switch_on();
  else
    SI4432_switch_off();
  SI4432_Deselect();
  si_enabled = en;
}

void SI4432_Set_Frequency ( uint32_t Freq ) {
  uint8_t hbsel;
//  if (0) shell_printf("Freq %q\r\n", Freq);
  if (Freq >= 480000000U) {
    hbsel = 1<<5;
    Freq>>=1;
  } else {
    hbsel = 0;
  }
  uint8_t sbsel = 1 << 6;
  uint32_t N = (Freq / SI4432_10MHZ - 24)&0x1F;
  uint32_t K = Freq % SI4432_10MHZ;
  uint32_t Carrier = (K<<2) / 625;
  uint8_t Freq_Band = N | hbsel | sbsel;

  SI4432_Write_Byte(SI4432_FREQBAND, Freq_Band );                          // Freq band must be written first !!!!!!!!!!!!
  SI4432_Write_Byte(SI4432_FREQCARRIER_H, (Carrier>>8) & 0xFF );
  SI4432_Write_Byte(SI4432_FREQCARRIER_L, Carrier & 0xFF  );
}

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

// !!!! FROM ili9341.c for disable it !!!! 
#define CS_LOW        palClearPad(GPIOB, GPIOB_LCD_CS)
#define CS_HIGH       palSetPad(GPIOB, GPIOB_LCD_CS)

// Software SPI use
#define SPI_CLK_HIGH   palSetPad(GPIOB, GPIOB_SPI_SCLK)
#define SPI_CLK_LOW    palClearPad(GPIOB, GPIOB_SPI_SCLK)

#define SPI_SDI_HIGH   palSetPad(GPIOB, GPIOB_SPI_MOSI)
#define SPI_SDI_LOW    palClearPad(GPIOB, GPIOB_SPI_MOSI)
#define SPI_RESET      palClearPort(GPIOB, (1<<GPIOB_SPI_SCLK)|(1<<GPIOB_SPI_MOSI))

#define SPI_SDO       ((palReadPort(GPIOB)>>GPIOB_SPI_MISO)&1)
#define SPI_portSDO   (palReadPort(GPIOB)&(1<<GPIOB_SPI_MISO))

#define SI_CS_LOW      palClearPad(GPIOA, GPIOA_SI_SEL)
#define SI_CS_HIGH     palSetPad(GPIOA, GPIOA_SI_SEL)

static uint32_t old_port_moder;
static uint32_t new_port_moder;
void SI4432_Select(void){
  CS_HIGH;
  // init for software bitbang
  GPIOB->MODER = new_port_moder;
  // Pull down SPI
  SPI_SDI_LOW;
  SPI_CLK_LOW;
}

void SI4432_Deselect(void){
  SI_CS_HIGH;
  // Restore hardware SPI
  GPIOB->MODER = old_port_moder;
}


static void SI4432_shiftOut(uint8_t val)
{
  uint8_t i = 0;
  do {
    if (val & 0x80)
      SPI_SDI_HIGH;
    SPI_CLK_HIGH;
    SPI_RESET;
    val<<=1;
  }while((++i) & 0x07);
}

static uint8_t SI4432_shiftIn(void)
{
  uint32_t value = 0;
  uint8_t i = 0;
  do {
    value<<=1;
    SPI_CLK_HIGH;
    value|=SPI_portSDO;
    SPI_CLK_LOW;
  }while((++i) & 0x07);
  return value>>GPIOB_SPI_MISO;
}

void SI4432_Init(void){
  // Store old port settings for software SPI mode
  old_port_moder = GPIOB->MODER;
  new_port_moder = old_port_moder & ~(PIN_MODE_ANALOG(GPIOB_SPI_SCLK)|PIN_MODE_ANALOG(GPIOB_SPI_MISO)|PIN_MODE_ANALOG(GPIOB_SPI_MOSI));
  new_port_moder|= PIN_MODE_OUTPUT(GPIOB_SPI_SCLK)|PIN_MODE_INPUT(GPIOB_SPI_MISO)|PIN_MODE_OUTPUT(GPIOB_SPI_MOSI);
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

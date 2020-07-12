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

// Define for use hardware SPI mode
//#define USE_HARDWARE_SPI_MODE

#define SI4432_10MHZ 10000000U
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

#ifdef USE_HARDWARE_SPI_MODE
// !!!! FROM ili9341.c !!!!
//*****************************************************
// SPI bus baud rate (PPL/BR_DIV)
//*****************************************************
#define	SPI_BR_DIV2   (0x00000000U)
#define	SPI_BR_DIV4   (SPI_CR1_BR_0)
#define	SPI_BR_DIV8   (SPI_CR1_BR_1)
#define	SPI_BR_DIV16  (SPI_CR1_BR_1|SPI_CR1_BR_0)
#define	SPI_BR_DIV32  (SPI_CR1_BR_2)
#define	SPI_BR_DIV64  (SPI_CR1_BR_2|SPI_CR1_BR_0)
#define	SPI_BR_DIV128 (SPI_CR1_BR_2|SPI_CR1_BR_1)
#define	SPI_BR_DIV256 (SPI_CR1_BR_2|SPI_CR1_BR_1|SPI_CR1_BR_0)

#define LCD_SPI_SPEED      SPI_BR_DIV2

#define SI4432_SPI_SPEED   SPI_BR_DIV8

#define SPI_BR_SET(br)  (SPI1->CR1 = (SPI1->CR1& ~(SPI_BR_DIV256))|br)

//*****************************************************
// SPI bus activity macros
//*****************************************************
// The RXNE flag is set depending on the FRXTH bit value in the SPIx_CR2 register:
// • If FRXTH is set, RXNE goes high and stays high until the RXFIFO level is greater or equal to 1/4 (8-bit).
#define SPI_RX_IS_NOT_EMPTY  (SPI1->SR&SPI_SR_RXNE)
#define SPI_RX_IS_EMPTY     (((SPI1->SR&SPI_SR_RXNE) == 0))

// The TXE flag is set when transmission TXFIFO has enough space to store data to send.
// 0: Tx buffer not empty, bit is cleared automatically when the TXFIFO level becomes greater than 1/2
// 1: Tx buffer empty, flag goes high and stays high until the TXFIFO level is lower or equal to 1/2 of the FIFO depth
#define SPI_TX_IS_NOT_EMPTY  (((SPI1->SR&(SPI_SR_TXE)) == 0))
#define SPI_TX_IS_EMPTY     (SPI1->SR&SPI_SR_TXE)

// When BSY is set, it indicates that a data transfer is in progress on the SPI (the SPI bus is busy).
#define SPI_IS_BUSY     (SPI1->SR & SPI_SR_BSY)

// Tx or Rx in process
#define SPI_IN_TX_RX    ((SPI1->SR & (SPI_SR_TXE | SPI_SR_RXNE)) == 0 || SPI_IS_BUSY)

#define SPI_WRITE_8BIT(data)  *(__IO uint8_t*)(&SPI1->DR) = (uint8_t) data
#define SPI_WRITE_16BIT(data) *(__IO uint16_t*)(&SPI1->DR) = (uint16_t) data

//*****************************************************
// SPI read data macros
//*****************************************************
#define SPI_READ_8BIT       *(__IO uint8_t*)(&SPI1->DR)
#define SPI_READ_16BIT      *(__IO uint16_t*)(&SPI1->DR)
#endif


#ifndef USE_HARDWARE_SPI_MODE
static uint32_t old_port_moder;
static uint32_t new_port_moder;
#endif

void SI4432_Select(void){
  CS_HIGH;
#ifdef USE_HARDWARE_SPI_MODE
  SPI_BR_SET(SI4432_SPI_SPEED);
#else
  // Init legs mode for software bitbang
  GPIOB->MODER = new_port_moder;
  // Pull down SPI
  SPI_SDI_LOW;
  SPI_CLK_LOW;
#endif
}

void SI4432_Deselect(void){
  SI_CS_HIGH;
#ifdef USE_HARDWARE_SPI_MODE
  SPI_BR_SET(LCD_SPI_SPEED);
#else
  // Restore hardware SPI
  GPIOB->MODER = old_port_moder;
#endif
}

static void SI4432_shiftOut(uint8_t val)
{
#ifdef USE_HARDWARE_SPI_MODE
  SPI_WRITE_8BIT(val);
  while (SPI_IS_BUSY) // drop rx and wait tx
    (void)SPI_READ_8BIT;
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
  SPI_WRITE_8BIT(0xFF);
  while (SPI_RX_IS_EMPTY); //wait rx data in buffer
  return SPI_READ_8BIT;
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

void SI4432_Init(void){
  // Store old port settings for software SPI mode
#ifndef USE_HARDWARE_SPI_MODE
  old_port_moder = GPIOB->MODER;
  new_port_moder = old_port_moder & ~(PIN_MODE_ANALOG(GPIOB_SPI_SCLK)|PIN_MODE_ANALOG(GPIOB_SPI_MISO)|PIN_MODE_ANALOG(GPIOB_SPI_MOSI));
  new_port_moder|= PIN_MODE_OUTPUT(GPIOB_SPI_SCLK)|PIN_MODE_INPUT(GPIOB_SPI_MISO)|PIN_MODE_OUTPUT(GPIOB_SPI_MOSI);
#endif

  SI4432_Select();
  // Switch off si4432
  SI4432_switch_off();

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
}

void SI4432_switch_off(void) {
  SI4432_Write_Byte(SI4432_GPIO0_CONF, 0x1d); // GPIO0 to VDD
  SI4432_Write_Byte(SI4432_GPIO1_CONF, 0x1f); // GPIO1 to GND
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

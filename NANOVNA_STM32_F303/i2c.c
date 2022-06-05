/*
 * Copyright (c) 2019-2020, Dmitry (DiSlord) dislordlive@gmail.com
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

#define VNA_I2C                         I2C1
#define I2C_CR2_SADD_7BIT_SHIFT         1
#define I2C_CR2_NBYTES_SHIFT            16

void i2c_set_timings(uint32_t timings) {
  VNA_I2C->CR1 &=~I2C_CR1_PE;
  VNA_I2C->TIMINGR = timings;
  VNA_I2C->CR1|= I2C_CR1_PE;
}

void i2c_start(void) {
  rccEnableI2C1(FALSE);
}

bool i2c_transfer(uint8_t addr, const uint8_t *w, size_t wn)//, uint8_t *r, size_t rn)
{
  VNA_I2C->CR1|= I2C_CR1_PE;
  if (wn) {
    VNA_I2C->CR2 = (addr << I2C_CR2_SADD_7BIT_SHIFT) | (wn << I2C_CR2_NBYTES_SHIFT);
    /*if (rn == 0)*/ VNA_I2C->CR2|= I2C_CR2_AUTOEND;
    VNA_I2C->CR2|= I2C_CR2_START;
    do {
      while ((VNA_I2C->ISR & I2C_ISR_TXE) == 0) {
        if (VNA_I2C->ISR & I2C_ISR_NACKF) {VNA_I2C->CR1&=~I2C_CR1_PE; return false;}  // NO ASK error
      }
      VNA_I2C->TXDR = *w++;
    } while (--wn);
    // not entirely sure this is really necessary. RM implies it will stall until it can write out the later bits
    //if (rn) while (!(i2c->ISR & I2C_ISR_TC));
  }
#if 0
  if (rn) {
    VNA_I2C->CR2 = (addr << I2C_CR2_SADD_7BIT_SHIFT) | (rn << I2C_CR2_NBYTES_SHIFT) | I2C_CR2_RD_WRN;
    VNA_I2C->CR2|= I2C_CR2_START;   // start transfer
    VNA_I2C->CR2|= I2C_CR2_AUTOEND; // important to do it afterwards to do a proper repeated start!
    for (size_t i = 0; i < rn; i++) {
      while((i2c->ISR & I2C_ISR_RXNE) == 0);
      r[i] = i2c->RXDR & 0xff;
    }
  }
#endif
  return true;
}
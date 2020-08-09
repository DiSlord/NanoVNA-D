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


#ifndef __SI4432_H__
#define __SI4432_H__

#define SI4432_DEV_TYPE                    0x00
#define SI4432_DEV_VERSION                 0x01
#define SI4432_DEV_STATUS                  0x02
#define SI4432_INT_STATUS1                 0x03
#define SI4432_INT_STATUS2                 0x04
#define SI4432_INT_ENABLE1                 0x05
#define SI4432_INT_ENABLE2                 0x06
#define SI4432_STATE                       0x07
#define SI4432_OPERATION_CONTROL           0x08
#define Si4432_CRYSTAL_OSC_LOAD_CAP        0x09
#define Si4432_UC_OUTPUT_CLOCK             0x0A
#define SI4432_GPIO0_CONF                  0x0B
#define SI4432_GPIO1_CONF                  0x0C
#define SI4432_GPIO2_CONF                  0x0D
#define SI4432_IOPORT_CONF                 0x0E
#define SI4432_IF_FILTER_BW                0x1C
#define SI4432_AFC_LOOP_GEARSHIFT_OVERRIDE 0x1D
#define SI4432_AFC_TIMING_CONTROL          0x1E
#define SI4432_CLOCK_RECOVERY_GEARSHIFT    0x1F
#define SI4432_CLOCK_RECOVERY_OVERSAMPLING 0x20
#define SI4432_CLOCK_RECOVERY_OFFSET2      0x21
#define SI4432_CLOCK_RECOVERY_OFFSET1      0x22
#define SI4432_CLOCK_RECOVERY_OFFSET0      0x23
#define SI4432_CLOCK_RECOVERY_TIMING_GAIN1 0x24
#define SI4432_CLOCK_RECOVERY_TIMING_GAIN0 0x25
#define SI4432_REG_RSSI                    0x26
#define SI4432_RSSI_THRESHOLD              0x27
#define SI4432_AFC_LIMITER                 0x2A
#define SI4432_AFC_CORRECTION_READ         0x2B
#define Si4432_OOK_COUNTER_VALUE_1         0x2C
#define Si4432_OOK_COUNTER_VALUE_2         0x2D
#define Si4432_SLICER_PEAK_HOLD            0x2E
#define SI4432_DATAACCESS_CONTROL          0x30
#define SI4432_EZMAC_STATUS                0x31
#define SI4432_HEADER_CONTROL1             0x32
#define SI4432_HEADER_CONTROL2             0x33
#define SI4432_PREAMBLE_LENGTH             0x34
#define SI4432_PREAMBLE_DETECTION          0x35
#define SI4432_SYNC_WORD3                  0x36
#define SI4432_SYNC_WORD2                  0x37
#define SI4432_SYNC_WORD1                  0x38
#define SI4432_SYNC_WORD0                  0x39
#define SI4432_TRANSMIT_HEADER3            0x3A
#define SI4432_TRANSMIT_HEADER2            0x3B
#define SI4432_TRANSMIT_HEADER1            0x3C
#define SI4432_TRANSMIT_HEADER0            0x3D
#define SI4432_PKG_LEN                     0x3E
#define SI4432_CHECK_HEADER3               0x3F
#define SI4432_CHECK_HEADER2               0x40
#define SI4432_CHECK_HEADER1               0x41
#define SI4432_CHECK_HEADER0               0x42
#define SI4432_RECEIVED_HEADER3            0x47
#define SI4432_RECEIVED_HEADER2            0x48
#define SI4432_RECEIVED_HEADER1            0x49
#define SI4432_RECEIVED_HEADER0            0x4A
#define SI4432_RECEIVED_LENGTH             0x4B
#define SI4432_CHARGEPUMP_OVERRIDE         0x58
#define SI4432_DIVIDER_CURRENT_TRIM        0x59
#define SI4432_VCO_CURRENT_TRIM            0x5A
#define SI4432_AGC_OVERRIDE                0x69
#define SI4432_TX_POWER                    0x6D
#define SI4432_TX_DATARATE1                0x6E
#define SI4432_TX_DATARATE0                0x6F
#define SI4432_MODULATION_MODE1            0x70
#define SI4432_MODULATION_MODE2            0x71
#define SI4432_FREQ_DEVIATION              0x72
#define SI4432_FREQ_OFFSET1                0x73
#define SI4432_FREQ_OFFSET2                0x74
#define SI4432_FREQBAND                    0x75
#define SI4432_FREQCARRIER_H               0x76
#define SI4432_FREQCARRIER_L               0x77
#define SI4432_FREQCHANNEL                 0x79
#define SI4432_CHANNEL_STEPSIZE            0x7A
#define SI4432_FIFO                        0x7F


void SI4432_Init(void);
void SI4432_Select(void);
void SI4432_Deselect(void);

void SI4432_switch(bool en);

void SI4432_Write_Byte(uint8_t ADR, uint8_t DATA);
uint8_t SI4432_Read_Byte( uint8_t ADR );
void SI4432_Set_Frequency ( uint32_t Freq );

#endif // __SI4432_H__

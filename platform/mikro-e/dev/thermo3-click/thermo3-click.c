/*
 * Copyright (c) 2016, Imagination Technologies Limited and/or its affiliated
 * group companies.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 * 	Thermo3 click driver.
 *	http://www.mikroe.com/click/thermo3/
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <pic32_i2c.h>
#include "thermo3-click.h"
/*---------------------------------------------------------------------------*/
void tmp102_init(void)
{
  pic32_i2c1_init();
  pic32_i2c1_set_frequency (I2C_FREQUENCY);
}
/*---------------------------------------------------------------------------*/
void tmp102_reg_select(uint8_t reg)
{
  pic32_i2c1_master_enable();
  pic32_i2c1_send_start();
    if(pic32_i2c1_send_byte (TMP102_REG_WRITE)) {
      pic32_i2c1_send_repeated_start();
      if(pic32_i2c1_send_byte (TMP102_REG_WRITE)) {
        printf("Failed the connection to Thermo3\n");
      }		
    }
  switch (reg) {
    case TMP102_TEMP:
      pic32_i2c1_send_byte (TMP102_TEMP);
      break;
    case TMP102_CONF:
      pic32_i2c1_send_byte (TMP102_CONF);
      break;
    case TMP102_TLOW:
      pic32_i2c1_send_byte (TMP102_TLOW);
      break;
    case TMP102_THIGH:
      pic32_i2c1_send_byte (TMP102_THIGH);
      break;
    default:
      printf("Invalid Register Address\n");
      break;
  }
  pic32_i2c1_send_stop();
  pic32_i2c1_master_disable();
}
/*---------------------------------------------------------------------------*/
void tmp102_reg_read(uint8_t *data)
{
  pic32_i2c1_master_enable();
  pic32_i2c1_send_start();
  /*command to TMP102 to Write data on bus */
  if(pic32_i2c1_send_byte (TMP102_REG_READ)) {
    printf("Failed the connection to Thermo3\n");	
  }
  pic32_i2c1_receive_byte(data);
  pic32_i2c1_receive_byte((data + 1));
  pic32_i2c1_send_stop();
  pic32_i2c1_master_disable();
}
/*---------------------------------------------------------------------------*/
void tmp102_reg_write(uint8_t msb, uint8_t lsb)
{
  pic32_i2c1_master_enable();
  pic32_i2c1_send_start();
  /*command to TMP102 to Read data from bus */
  pic32_i2c1_send_byte (TMP102_REG_WRITE);
  pic32_i2c1_send_byte(msb);
  pic32_i2c1_send_byte(lsb);
  pic32_i2c1_send_stop();
  pic32_i2c1_master_disable();
}
/*---------------------------------------------------------------------------*/
float tmp102_read_temp(void)
{
  uint8_t data[2] = { 0, 0 };
  float temp_c = 0;
  tmp102_reg_read(data);
  /* data[1] having LSB of Temp. Register */
  if(data[1] & 0x10) {
    temp_c += 0.06250;
  }
  if(data[1] & 0x20) {
    temp_c += 0.12500;
  }
  if(data[1] & 0x40) {
    temp_c += 0.25000;
  }
  if(data[1] & 0x80) {
    temp_c += 0.50000;
  }
  /* data[0] having MSB of Temp. Register */
  if((data[0] & 0x80) != 0x80) {
    temp_c += ((float) data[0]); 
  }
  if((data[0] & 0x80) == 0x80) {
    data[0] = data[0] - 0x01;
    data[0] = ~data[0];
    temp_c = temp_c - ((float) data[0]);
  }
  return temp_c;
}
/*---------------------------------------------------------------------------*/

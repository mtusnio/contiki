/*
 * Copyright (c) 2017, Imagination Technologies Limited and/or its affiliated
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
 * \addtogroup pic32 PIC32 Contiki Port
 *
 * @{
 */

/**
 * \file   pic32_rtc.c
 * \brief  RTC driver for PIC32MX
 * \author Francois Berder <francois.berder@imgtec.com>
 * \date   2017-17-02
 */


#ifdef __USE_RTC__

#include <p32xxxx.h>
#include <pic32_irq.h>
#include <pic32_rtc.h>
/*---------------------------------------------------------------------------*/
static uint32_t
bcd_to_binary(uint8_t a, uint8_t b)
{
  return a * 10 + b;
}
/*---------------------------------------------------------------------------*/
static uint32_t
binary_to_bcd(uint32_t a)
{
  uint32_t decimal = a / 10;
  uint32_t digit = a - decimal * 10;

  return (decimal << 4) | digit;
}
/*---------------------------------------------------------------------------*/
int8_t
pic32_rtc_init(void)
{
  SYSKEY = 0;
  SYSKEY = 0xaa996655;
  SYSKEY = 0x556699aa;

  OSCCONSET = _OSCCON_SOSCEN_MASK;  /* Enable external 32.768kHz crystal */
  RTCCON = _RTCCON_RTCWREN_MASK;    /* Enable write to RTCCON register */

  SYSKEY = 0;

  RTCALRM = 0;      /* Disable alarm */
  RTCTIME = 0;
  RTCDATE = 0;

  /* Turn on module and disable further writes to RTCCON */
  RTCCON = _RTCCON_ON_MASK;

  /* Wait for the clock of RTC module */
  while(!(RTCCON & _RTCCON_RTCCLKON_MASK)) {
  }

  return 0;
}
/*---------------------------------------------------------------------------*/
struct date
pic32_rtc_get_date(void)
{
  struct date date;

  /* Make some copies to avoid rollover while reading registers */
  uint32_t rtcdate = RTCDATE;
  uint32_t rtctime = RTCTIME;

  date.year = bcd_to_binary(
      (rtcdate & _RTCDATE_YEAR10_MASK) >> _RTCDATE_YEAR10_POSITION,
      (rtcdate & _RTCDATE_YEAR01_MASK) >> _RTCDATE_YEAR01_POSITION);

  date.month = bcd_to_binary(
      (rtcdate & _RTCDATE_MONTH10_MASK) >> _RTCDATE_MONTH10_POSITION,
      (rtcdate & _RTCDATE_MONTH01_MASK) >> _RTCDATE_MONTH01_POSITION);

  /*
   * RTC Module stores month in range 1..12
   * Decrement to get it in range 0..11 (easier for indexing)
   */
  --date.month;

  date.day = bcd_to_binary(
      (rtcdate & _RTCDATE_DAY10_MASK) >> _RTCDATE_DAY10_POSITION,
      (rtcdate & _RTCDATE_DAY01_MASK) >> _RTCDATE_DAY01_POSITION);

  date.weekday = bcd_to_binary(0,
      (rtcdate & _RTCDATE_WDAY01_MASK) >> _RTCDATE_WDAY01_POSITION);

  date.hour = bcd_to_binary(
      (rtctime & _RTCTIME_HR10_MASK) >> _RTCTIME_HR10_POSITION,
      (rtctime & _RTCTIME_HR01_MASK) >> _RTCTIME_HR01_POSITION);

  date.minute = bcd_to_binary(
      (rtctime & _RTCTIME_MIN10_MASK) >> _RTCTIME_MIN10_POSITION,
      (rtctime & _RTCTIME_MIN01_MASK) >> _RTCTIME_MIN01_POSITION);

  date.second = bcd_to_binary(
      (rtctime & _RTCTIME_SEC10_MASK) >> _RTCTIME_SEC10_POSITION,
      (rtctime & _RTCTIME_SEC01_MASK) >> _RTCTIME_SEC01_POSITION);

  return date;
}
/*---------------------------------------------------------------------------*/
int8_t
pic32_rtc_set_date(struct date date)
{
  uint32_t rtccon_state = RTCCON;

  ++date.month;

  if(date.year > 99
  || date.month == 0 || date.month > 12
  || date.weekday > 6
  || date.day == 0 || date.day > 31
  /* April, June, September and November have only 30 days */
  || ((date.month == 4 || date.month == 6 || date.month == 9 || date.month == 11) && date.day > 30)
  /*
   * Since the year range in RTC module is from 2000 to 2099, February has 29 days
   * when the year is divisible by 4.
   */
  || (date.month == 2 && date.day > 28 + (date.year & 0x3 ? 0 : 1))
  || date.hour > 23
  || date.minute > 59
  || date.second > 59) {
    return -1;
  }

  SYSKEY = 0;
  SYSKEY = 0xaa996655;
  SYSKEY = 0x556699aa;

  /* Enable write to RTCCON register to clear ON bit */
  RTCCONSET = _RTCCON_RTCWREN_MASK;

  SYSKEY = 0;

  RTCCONCLR = _RTCCON_ON_MASK;

  RTCDATE = binary_to_bcd(date.year) << _RTCDATE_YEAR01_POSITION;
  RTCDATE |= binary_to_bcd(date.month) << _RTCDATE_MONTH01_POSITION;
  RTCDATE |= binary_to_bcd(date.day) << _RTCDATE_DAY01_POSITION;
  RTCDATE |= binary_to_bcd(date.weekday) << _RTCDATE_WDAY01_POSITION;

  RTCTIME = binary_to_bcd(date.hour) << _RTCTIME_HR01_POSITION;
  RTCTIME |= binary_to_bcd(date.minute) << _RTCTIME_MIN01_POSITION;
  RTCTIME |= binary_to_bcd(date.second) << _RTCTIME_SEC01_POSITION;

  RTCCON = rtccon_state;

  /*
   * Ensure that RTCCON is locked in case rtccon_state still
   * enables write to RTCCON
   */
  RTCCONCLR = _RTCCON_RTCWREN_MASK;

  return 0;
}
/*---------------------------------------------------------------------------*/
int8_t
pic32_rtc_release(void)
{
  ASM_DIS_INT;

  SYSKEY = 0;
  SYSKEY = 0xaa996655;
  SYSKEY = 0x556699aa;

  /* Enable write to RTCCON register to clear it */
  RTCCONSET = _RTCCON_RTCWREN_MASK;
  SYSKEY = 0;

  ASM_EN_INT;

  RTCCON = 0; /* Stop module and disable further writes to RTCCON */

  /*
   * SOSC bit of OSCCON register is not cleared because other peripherals
   * may rely on the external 32.768kHz crystal.
   */

  return 0;
}

#endif  /* __USE_RTC__ */

/** @} */

/*
 * Copyright (c) 2017, Imagination Technologies Limited and/or its
 * affiliated group companies.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 * 	A test application for RTC peripheral of PIC32.
 */

#include <contiki.h>
#include <stdio.h>
#include <pic32_rtc.h>
/*---------------------------------------------------------------------------*/
static const char *weekday_str[] = {
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday",
  "Sunday"
};
static const char *month_str[] = {
  "January",
  "Februay",
  "March",
  "April",
  "May",
  "June",
  "July",
  "August",
  "September",
  "October",
  "November",
  "December"
};
/*---------------------------------------------------------------------------*/
PROCESS(test_motion, "PIC32 - RTC");
AUTOSTART_PROCESSES(&test_motion);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_motion, ev, data)
{
  PROCESS_BEGIN();

  static struct etimer et;

  pic32_rtc_init();
  struct date initial = {
    .year = 17,
    .month = 3,
    .weekday = 2,
    .day = 1,
    .hour = 23,
    .minute = 12,
    .second = 54
  };
  pic32_rtc_set_date(initial);

  while(1) {
    struct date d = pic32_rtc_get_date();
    printf("%s %u %s %u, %u:%u:%u\n", weekday_str[d.weekday], d.day, month_str[d.month], d.year + 2000, d.hour, d.minute, d.second);
    etimer_set(&et, CLOCK_SECOND);
    PROCESS_WAIT_EVENT();
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

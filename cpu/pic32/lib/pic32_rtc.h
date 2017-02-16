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


#ifndef INCLUDE_PIC32_RTC_H_
#define INCLUDE_PIC32_RTC_H_

#ifdef __USE_RTC__

#include <stdint.h>

struct date {
    uint8_t year;     /**< in range 0..99 */
    uint8_t month;    /**< in range 0-11 */
    uint8_t day;      /**< in range 1-31 (depends on month) */
    uint8_t weekday;  /**< in range 0-6 */
    uint8_t hour;     /**< in range 0-23 */
    uint8_t minute;   /**< in range 0-59 */
    uint8_t second;   /**< in range 0-59 */
};

int8_t pic32_rtc_init(void);
struct date pic32_rtc_get_date(void);
int8_t pic32_rtc_set_date(struct date date);
int8_t pic32_rtc_release(void);

#endif /* __USE_RTC__ */

#endif /* INCLUDE_PIC32_RTC_H_ */

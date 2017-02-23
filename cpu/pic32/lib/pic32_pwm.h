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

/*
 * PWM driver of PIC32.
 *
 * It configures timer and output compare modules.
 * All duty_cycle and period parameters are in nanoseconds.
 */

#ifndef INCLUDE_PIC32_PWM_H_
#define INCLUDE_PIC32_PWM_H_

#ifdef __USE_PWM__

#include <stdint.h>

#define PWM_DEF(XX)                                                             \
    int8_t pic32_pwm##XX##_init(uint32_t period);                               \
    int8_t pic32_pwm##XX##_set_duty_cycle(uint32_t duty_cycle);                 \
    uint32_t pic32_pwm##XX##_get_duty_cycle(void);                              \
    uint32_t pic32_pwm##XX##_get_maximum_duty_cycle(void);                      \
    int8_t pic32_pwm##XX##_enable(void);                                        \
    int8_t pic32_pwm##XX##_disable(void);                                       \
    int8_t pic32_pwm##XX##_release(void);

#ifdef __32MX470F512H__
  #ifdef __USE_PWM_5__
  PWM_DEF(5)
  #endif /* __USE_PWM_5__ */
#endif /* __32MX470F512H__ */

#endif /* __USE_PWM__ */

#endif /* INCLUDE_PIC32_PWM_H_ */

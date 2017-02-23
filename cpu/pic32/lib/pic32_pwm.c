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
 * \file   pic32_pwm.c
 * \brief  PWM driver for PIC32MX
 * \author Francois Berder <francois.berder@imgtec.com>
 * \date   2012-02-20
 */

#ifdef __USE_PWM__

#include <p32xxxx.h>
#include <pic32_clock.h>
#include <pic32_pwm.h>

#define PRESCALER_COUNT   (8)

/* Number of bits in the decimal part in fixed-point format */
#define DECIMAL_COUNT     (4)

/* Peripheral clock period in nanoseconds in fixed-point format */
#define PBCLK_PERIOD      (uint32_t)((1000 * 1000 * 1000) / (pic32_clock_get_peripheral_clock() >> DECIMAL_COUNT))

/*----------------------------------------------------------------------------------------------*/
static const uint32_t prescaler_values[PRESCALER_COUNT] = {1, 2, 4, 8, 16, 32, 64, 256};
/*----------------------------------------------------------------------------------------------*/
/**
 * \brief Configure timer parameters
 *
 * PR and prescalers of timer need to be found such that the period of the timer is as close as possible
 * from the desired period. At the same time, PR needs to be as high as possible such that the resolution
 * is as high as possible.
 *
 * Note that all computations involving periods are done in fixed-point format. The reason is that computing
 * the period of the peripheral base clock in nanoseconds might not result in an integer and this leads
 * to errors in the computation of PR. The DECIMAL_COUNT macro represents the number of bits for the
 * decimal part.
 *
 * \param[in] period in nanoseconds
 * \param[out] prescaler_index
 * \param[out] pr Timer period
 * \return Error code
 * \retval 0 if successful
 * \retval 1 if output parameters are NULL
 * \retval 2 if no suitable prescaler can be found,
 * \retval 3 if the period error is too big.
 */
static uint8_t
configure_timer(uint32_t period, uint8_t *prescaler_index, volatile unsigned int *pr)
{
  const uint32_t pb_period = PBCLK_PERIOD;
  uint32_t max_error, best_pr = 0;
  int8_t minimum_prescaler_index = 0, i = 0;

  if(prescaler_index == NULL || pr == NULL) {
    return 1;
  }

  /* Convert period into fixed-point unsigned integer format */
  period <<= DECIMAL_COUNT;

  /*
   * 1. Find minimum prescaler that we can use.
   * Since we use 16bit timer, PR maximum value is 65535.
   *
   * period of the timer = (PR + 1) * prescaler * pb_period
   *
   * Iterate over all prescalers in ascending order and find minimum prescaler such that:
   *
   *      desired_period
   *      -------------- < prescaler * pb_period
   *           65536
   */
  while(minimum_prescaler_index < PRESCALER_COUNT) {
    uint32_t max_prescaler_period = pb_period * prescaler_values[minimum_prescaler_index];

    if((period >> 16) < max_prescaler_period) {
      break;
    }
    ++minimum_prescaler_index;
  }
  if(minimum_prescaler_index == PRESCALER_COUNT) {
    return 2;
  }

  /*
   * 2. Find PR given prescaler.
   * Iterate over suitable prescaler, compute PR+1 and PR and their respective
   * periods:
   *
   * period'= (PR + 2) * pb_period * prescaler
   * period = (PR + 1) * pb_period * prescaler
   *
   * Since period' will always be greater than desired period, and period will
   * always be lower than the desired period, the computation of the error becomes:
   *
   * error' = period' - desired_period
   * error = desired_period - period
   *
   * If min(error,error') is less than max_error, then either PR or PR+1 is the
   * best pr.
   *
   * max error is a percentage of the desired period:
   *
   *              period
   * max error = ---------
   *              (1 << n)
   *
   * Since n = 5, max error = period / 32 = period * (1/32) = period * 3.125%
   */
  max_error = period >> 5;
  for(i = minimum_prescaler_index; i < PRESCALER_COUNT; ++i) {
    uint32_t pr[2], actual_period[2], error[2];
    uint32_t err;

    /* Compute PR+1 and PR */
    pr[0] = (period / (pb_period * prescaler_values[i]));
    pr[1] = pr[0] - 1;

    /* Compute periods */
    actual_period[0] = (pr[0] + 1) * pb_period * prescaler_values[i];
    actual_period[1] = actual_period[0] - pb_period * prescaler_values[i];

    /* Compute error of (PR, prescaler) */
    error[0] = actual_period[0] - period;
    error[1] = period - actual_period[1];
    err = error[0] < error[1] ? error[0] : error[1];

    /*
     * Since we go through the list of prescalers in ascending order, the first
     * (PR, prescaler) which gives an acceptable error is the best because PR
     * will be as high as possible, and this gives the best resolution.
     */
    if(err < max_error) {
      best_pr = err == error[0] ? pr[0] : pr[1];
      break;
    }
  }
  if (i == PRESCALER_COUNT) {
    return 3;
  }

  *pr = best_pr;
  *prescaler_index = i;

  return 0;
}
/*----------------------------------------------------------------------------------------------*/
#define PWM(XX, YY)                                                                                                 \
    int8_t pic32_pwm##XX##_init(uint32_t period)                                                                    \
    {                                                                                                               \
      uint8_t prescaler_index;                                                                                      \
                                                                                                                    \
      OC##XX##CON = 0;                                                                                              \
      T##YY##CON = 0;                                                                                               \
                                                                                                                    \
      uint8_t ret = configure_timer(period, &prescaler_index, &PR##YY);                                             \
      if(ret != 0) {                                                                                                \
        return ret;                                                                                                 \
      }                                                                                                             \
      T##YY##CONbits.TCKPS = prescaler_index;                                                                       \
      TMR##YY = 0;                                                                                                  \
                                                                                                                    \
      /* Set initial duty cycle to 0 */                                                                             \
      OC##XX##RS = 0;                                                                                               \
      OC##XX##R = 0;                                                                                                \
                                                                                                                    \
      /* Configure Output Compare module in PWM mode without fault pin */                                           \
      OC##XX##CON = 0b110;                                                                                          \
                                                                                                                    \
      OC##XX##CONbits.OCTSEL = (YY) & 0x1;                                                                          \
                                                                                                                    \
      /* Disable interrupts */                                                                                      \
      IEC0CLR = _IEC0_OC##XX##IE_MASK | _IEC0_T##YY##IE_MASK;                                                       \
                                                                                                                    \
      return ret;                                                                                                   \
    }                                                                                                               \
    int8_t pic32_pwm##XX##_set_duty_cycle(uint32_t duty_cycle)                                                      \
    {                                                                                                               \
      duty_cycle <<= DECIMAL_COUNT; /* Convert to fixed-point format */                                             \
      duty_cycle /= (prescaler_values[T##YY##CONbits.TCKPS] * PBCLK_PERIOD);                                        \
      OC##XX##RS = duty_cycle;                                                                                      \
      return 0;                                                                                                     \
    }                                                                                                               \
    uint32_t pic32_pwm##XX##_get_duty_cycle(void)                                                                   \
    {                                                                                                               \
      uint32_t duty_cycle = OC##XX##RS * prescaler_values[T##YY##CONbits.TCKPS] * PBCLK_PERIOD;                     \
      duty_cycle >>= DECIMAL_COUNT; /* Discard decimal bits */                                                      \
      return duty_cycle;                                                                                            \
    }                                                                                                               \
    uint32_t pic32_pwm##XX##_get_maximum_duty_cycle(void)                                                           \
    {                                                                                                               \
      uint32_t max_duty_cycle = (PR##YY + 1) * prescaler_values[T##YY##CONbits.TCKPS] * PBCLK_PERIOD;               \
      max_duty_cycle >>= DECIMAL_COUNT; /* Discard decimal bits */                                                  \
      return max_duty_cycle;                                                                                        \
    }                                                                                                               \
    int8_t pic32_pwm##XX##_enable(void)                                                                             \
    {                                                                                                               \
      T##YY##CONSET = _T##YY##CON_TON_MASK;                                                                         \
      OC##XX##CONSET = _OC##XX##CON_ON_MASK;                                                                        \
      return 0;                                                                                                     \
    }                                                                                                               \
    int8_t pic32_pwm##XX##_disable(void)                                                                            \
    {                                                                                                               \
      T##YY##CONCLR = _T##YY##CON_TON_MASK;                                                                         \
      OC##XX##CONCLR = _OC##XX##CON_ON_MASK;                                                                        \
      return 0;                                                                                                     \
    }                                                                                                               \
    int8_t pic32_pwm##XX##_release(void)                                                                            \
    {                                                                                                               \
      /* For now, just disable the Timer and the Output Compare modules */                                          \
      OC##XX##CON = 0;                                                                                              \
      T##YY##CON = 0;                                                                                               \
      return 0;                                                                                                     \
    }

#ifdef __32MX470F512H__
  #ifdef __USE_PWM_5__
  PWM(5, 3)     /* Use timer 3 */
  #endif /* __USE_PWM_5__ */
#endif

#endif /* __USE_PWM__ */

/** @} */

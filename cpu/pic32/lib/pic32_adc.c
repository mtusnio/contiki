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
 * \file   pic32_adc.c
 * \brief  ADC interface for PIC32MX
 * \author Michal Tusnio <michal.tusnio@imgtec.com>
 * \date   2017-03-02
 */

#ifdef __USE_ADC__

#include <pic32_adc.h>
#include <pic32_gpio.h>
#include <p32xxxx.h>

#define ADC_PIN_SHIFT   (4)
#define ADC_PIN_MAX     (15)

#define ADC_MAX_SAMPLE_RATE 1000000

static uint8_t PBCLK_SCALERS[4] = {1, 2, 4, 8};

static uint8_t
calculate_period_bit(uint32_t sample_rate)
{
    uint32_t period = 2 * (1000 * 1000 * 1000)/pic32_clock_get_peripheral_clock();

}

/*---------------------------------------------------------------------------*/
int8_t
pic32_adc_init(uint32_t sample_rate)
{
  if(sample_rate > ADC_MAX_SAMPLE_RATE) {
    return -ADC_BAD_ARGUMENT;
  }

  AD1CON1CLR = _AD1CON1_ON_MASK;

  AD1CON1 = _AD1CON1_SSRC0_MASK | _AD1CON1_SSRC1_MASK | _AD1CON1_SSRC2_MASK;
  AD1CON2 = 0;
  AD1CON3 = calculate_period_bit(sample_rate)
            | _AD1CON3_SAMC0_MASK | _AD1CON3_SAMC1_MASK | _AD1CON3_SAMC2_MASK
            | _AD1CON3_SAMC3_MASK;
  AD1CHS = 0;
  AD1CSSL = 0;
  AD1CON1SET = _AD1CON1_ON_MASK;
  return ADC_NO_ERROR;
}

/*---------------------------------------------------------------------------*/
int8_t
pic32_adc_get_value(uint8_t pin, uint16_t * value)
{
  if(!value) {
    return -ADC_BAD_ARGUMENT;
  }

  if(pin > ADC_PIN_MAX) {
    return -ADC_BAD_ARGUMENT;
  }

  AD1CHS = (pin + ADC_PIN_SHIFT) << _AD1CON1_CLRASAM_MASK;
  AD1CON1bits.SAMP = 1;
  while( AD1CON1bits.SAMP || !AD1CON1bits.DONE );
  *value = ADC1BUF0;
  return ADC_NO_ERROR;
}

/*---------------------------------------------------------------------------*/
int8_t
pic32_adc_release(void)
{
  AD1CON1CLR = _AD1CON1_ON_MASK;
  return ADC_NO_ERROR;
}

#endif /* __USE_ADC__ */

/** @} */

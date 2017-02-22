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
 * \addtogroup pic32 PIC32 Contiki Port
 *
 * @{
 */

/**
 * \file   lpm.c
 * \brief  Low power management for PIC32MX
 * \author Francois Berder <francois.berder@imgtec.com>
 * \date   2017-09-02
 */

#ifdef __USE_LPM__

#include <stddef.h>
#include <p32xxxx.h>
#include "lib/list.h"
#include "lpm.h"

LIST(peripherals_list);
/*---------------------------------------------------------------------------*/
static void
power_down_peripherals(void)
{
  lpm_registered_peripheral_t *peripheral;
  for(peripheral = list_head(peripherals_list); peripheral != NULL;
      peripheral = peripheral->next) {
    if(peripheral->power_down) {
      peripheral->power_down();
    }
  }
}
/*---------------------------------------------------------------------------*/
static void
power_up_peripherals(void)
{
  lpm_registered_peripheral_t *peripheral;
  for(peripheral = list_head(peripherals_list); peripheral != NULL;
      peripheral = peripheral->next) {
    if(peripheral->power_up) {
      peripheral->power_up();
    }
  }
}
/*---------------------------------------------------------------------------*/
void
lpm_enter(void)
{
  power_down_peripherals();

  asm volatile("wait");

  power_up_peripherals();
}
/*---------------------------------------------------------------------------*/
void
lpm_register_peripheral(lpm_registered_peripheral_t *peripheral)
{
  list_add(peripherals_list, peripheral);
}
/*---------------------------------------------------------------------------*/
void
lpm_unregister_peripheral(lpm_registered_peripheral_t *peripheral)
{
  list_remove(peripherals_list, peripheral);
}
/*---------------------------------------------------------------------------*/
void
lpm_init(void)
{
  list_init(peripherals_list);

  SYSKEY = 0;
  SYSKEY = 0xaa996655;
  SYSKEY = 0x556699aa;

  /*
   * Enable Idle Mode when wait instruction is executed.
   * Idle mode is chosen instead of sleep to allow peripherals using the
   * PBCLK to wake up the CPU.
   */
  OSCCONCLR = 1 << _OSCCON_SLPEN_POSITION;

  SYSKEY = 0;
}

#endif /* __USE_LPM__ */

/** @} */

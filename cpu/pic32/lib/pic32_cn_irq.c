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
 * \file   pic32_cn_irq.c
 * \brief  IRQ change notice driver for PIC32MX
 * \author Michal Tusnio <michal.tusnio@imgtec.com>
 * \date   2017-03-01
 */

#ifdef __USE_CN_IRQ__

#include <pic32_cn_irq.h>
#include <p32xxxx.h>
#include "lib/list.h"
#include "lib/memb.h"

#define IRQ_ENTRIES 32

struct irq_callback_entry {
    struct irq_callback_entry * next;
    void (*callback)(void);
};

LIST(irq_callbacks);
MEMB(irq_callbacks_memb, struct irq_callback_entry, IRQ_ENTRIES);

static int8_t irq_initialised = 0;

/*---------------------------------------------------------------------------*/
int8_t
pic32_cn_irq_init(void)
{
  if(irq_initialised)
    return -IRQ_UNINITIALISED;

  memb_init(&irq_callbacks_memb);
  list_init(irq_callbacks);
  irq_initialised = 1;
  return IRQ_NO_ERROR;
}
/*---------------------------------------------------------------------------*/
int8_t
pic32_cn_irq_add_callback(void (*callback)(void))
{
  if(!callback) {
    return -IRQ_BAD_ARGUMENT;
  }

  if(!irq_initialised) {
    return -IRQ_UNINITIALISED;
  }

  struct irq_callback_entry * s;
  for(s = list_head(irq_callbacks);
      s != NULL;
      s = list_item_next(s)) {
    if(s->callback == callback) {
      return -IRQ_ALREADY_IN_LIST;
    }
  }

  struct irq_callback_entry * m = memb_alloc(&irq_callbacks_memb);
  m->callback = callback;
  list_insert(irq_callbacks, NULL, m);
  return IRQ_NO_ERROR;
}
/*---------------------------------------------------------------------------*/
int8_t
pic32_cn_irq_remove_callback(void (*callback)(void))
{
  if(!irq_initialised) {
    return -IRQ_UNINITIALISED;
  }

  struct irq_callback_entry * s;

  for(s = list_head(irq_callbacks);
      s != NULL;
      s = list_item_next(s)) {
    if(s->callback == callback) {
      list_remove(irq_callbacks, s);
      return IRQ_NO_ERROR;
    }
  }

  return -IRQ_NOT_IN_LIST;
}
/*---------------------------------------------------------------------------*/
int8_t
pic32_cn_irq_release(void)
{
  if(!irq_initialised) {
    return -IRQ_UNINITIALISED;
  }

  struct irq_callback_entry * l;
  while((l = list_pop(irq_callbacks)) != NULL) {
    memb_free(&irq_callbacks_memb, l);
  };

  irq_initialised = 0;
  return IRQ_NO_ERROR;
}
/*---------------------------------------------------------------------------*/
ISR(_CHANGE_NOTICE_VECTOR)
{
  struct irq_callback_entry * s;

  for(s = list_head(irq_callbacks);
      s != NULL;
      s = list_item_next(s)) {
    if(s->callback)
        s->callback();
  }
}

#endif

/** @} */

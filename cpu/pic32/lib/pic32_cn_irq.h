/*
 * Contiki PIC32 Port project
 *
 * Copyright (c) 2012,
 *  Scuola Superiore Sant'Anna (http://www.sssup.it) and
 *  Consorzio Nazionale Interuniversitario per le Telecomunicazioni
 *  (http://www.cnit.it).
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
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * \addtogroup pic32 PIC32 Contiki Port
 *
 * @{
 */

/**
 * \file   pic32_cn_irq.h
 * \brief  IRQ change notice driver for PIC32MX
 * \author Michal Tusnio <michal.tusnio@imgtec.com>
 * \date   2017-03-01
 */

#ifndef INCLUDE_PIC32_CN_IRQ_H_
#define INCLUDE_PIC32_CN_IRQ_H_

#ifdef __USE_CN_IRQ__

#include <stdint.h>
#include <pic32_irq.h>

#define IRQ_NO_ERROR            (0)
#define IRQ_BAD_ARGUMENT        (1)
#define IRQ_UNINITIALISED       (2)
#define IRQ_NOT_IN_LIST         (3)
#define IRQ_ALREADY_IN_LIST     (4)


int8_t pic32_cn_irq_init(void);
int8_t pic32_cn_irq_add_callback(void (*callback)(void));
int8_t pic32_cn_irq_remove_callback(void (*callback)(void));
int8_t pic32_cn_irq_release(void);
#endif

#endif /* INCLUDE_PIC32_CN_IRQ_H_ */

/** @} */

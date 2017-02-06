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

#ifndef LPM_H
#define LPM_H

#include <stdint.h>

typedef struct lpm_registered_peripheral {
    struct lpm_registered_peripheral *next;
    int8_t (*power_down)(void);
    int8_t (*power_up)(void);
} lpm_registered_peripheral_t;

/**
 * @brief Enter low power management mode
 *
 * This function performs three actions:
 *  1. Power down all registered peripherals
 *  2. Execute wait instruction
 *  3. Power up all registered peripherals
 *
 */
void lpm_enter(void);

/**
 * @brief Register a peripheral to the Low-power management module
 *
 * Adds this peripheral to the list of peripherals that can be powered
 * up/down. Notice that these functions can be NULL if no implementation
 * is required for the peripheral.
 *
 * @param[in] peripheral
 */
void lpm_register_peripheral(lpm_registered_peripheral_t *peripheral);

/**
 * @brief Remove a peripheral from the list
 *
 * @param[in] peripheral
 */
void lpm_unregister_peripheral(lpm_registered_peripheral_t *peripheral);

/**
 * @brief Initialise the list of registered peripherals.
 */
void lpm_init(void);

#endif /* LPM_H */

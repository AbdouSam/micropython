/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <string.h>
#include <stdint.h>
#include "py/stream.h"
#include "py/mphal.h"
#include "board.h"
#include "timer.h"
#include "delay.h"
#include "interrupt.h"

#define SYS_TICK_FREQ (1000) // 1kHz

static int interrupt_char;

void mp_hal_init(void) {
    MP_STATE_PORT(keyboard_interrupt_obj) = mp_obj_new_exception(&mp_type_KeyboardInterrupt);
}

// use timer 1 as system tick at 1 ms.
void mp_hal_init_ticks(void) {
    init_timer1(SYS_TICK_FREQ, TMR_PRESCALE_1, 0);
}

mp_uint_t mp_hal_ticks_ms(void) {
    return interrupt_tick_get();
}

void mp_hal_delay_ms(mp_uint_t ms) {
    delay_ms(ms);
}

void mp_hal_set_interrupt_char(int c) {
    interrupt_char = c;
}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    if ((poll_flags & MP_STREAM_POLL_RD) && uart_rx_any()) {
        ret |= MP_STREAM_POLL_RD;
    }
    return ret;
}

int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        if (uart_rx_any()) {
            return uart_rx_char();
        }
    }
}

void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}

void mp_hal_stdout_tx_strn(const char *str, size_t len) {
    for (; len > 0; --len) {
        uart_tx_char(*str++);
    }
}

void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {

    for (; len > 0; --len) {
        if (*str == '\n') {
            uart_tx_char('\r');
        }
        uart_tx_char(*str++);
    }
}


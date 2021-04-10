/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
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
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "bsp/board.h"
#include "pico_ntsc_grph.h"
#include "tusb.h"

void print_greeting(void);
void led_blinking_task(void);
extern void hid_task(void);

#define GX(x)   (x * 8)
#define GY(y)   (y * 8)

int cx = 0;
int cy = 4;

int main(void) {

    // initialize video and LED GPIO
    init_video_and_led_GPIO();
    // init stdio
    stdio_init_all();
    // initialize and start PWM interrupt by 64us period
    enable_PWM_interrupt( );
    gvram_clear();
    gvram_strings(0, 0, "Hello!", WDOT);

    board_init();
    print_greeting();

//    return 0;

    tusb_init();

    while (1) {
        // tinyusb host task
        tuh_task();
        led_blinking_task();

#if CFG_TUH_HID_KEYBOARD || CFG_TUH_HID_MOUSE
        hid_task();
#endif
    }

    return 0;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+
#if CFG_TUH_HID_KEYBOARD

CFG_TUSB_MEM_SECTION static hid_keyboard_report_t usb_keyboard_report;
uint8_t const keycode2ascii[128][2] = {HID_KEYCODE_TO_ASCII};

// look up new key in previous keys
static inline bool find_key_in_report(hid_keyboard_report_t const *p_report, uint8_t keycode) {
    for (uint8_t i = 0; i < 6; i++) {
        if (p_report->keycode[i] == keycode) return true;
    }

    return false;
}

static inline void process_kbd_report(hid_keyboard_report_t const *p_new_report) {
    static hid_keyboard_report_t prev_report = {0, 0, {0}}; // previous report to check key released

    //------------- example code ignore control (non-printable) key affects -------------//
    for (uint8_t i = 0; i < 6; i++) {
        if (p_new_report->keycode[i]) {
            if (find_key_in_report(&prev_report, p_new_report->keycode[i])) {
                // exist in previous report means the current key is holding
            } else {
                // not existed in previous report means the current key is pressed
                bool const is_shift =
                        p_new_report->modifier & (KEYBOARD_MODIFIER_LEFTSHIFT | KEYBOARD_MODIFIER_RIGHTSHIFT);
                uint8_t ch = keycode2ascii[p_new_report->keycode[i]][is_shift ? 1 : 0];
                putchar(ch);
                if (ch == '\r') putchar('\n'); // added new line for enter key
                gvram_put_char(GX(cx), GY(cy), ch, WDOT);
                cx++;
                if (cx > VRAM_W) {
                    cx = 0;
                    cy++;
                    if (cy > VRAM_H) {
                        cy = 4;
                    }
                }
                if (ch == '\r') {
                    cy++; cx = 0;
                }
                fflush(stdout); // flush right away, else nanolib will wait for newline
            }
        }
        // TODO example skips key released
    }

    prev_report = *p_new_report;
}

void tuh_hid_keyboard_mounted_cb(uint8_t dev_addr) {
    // application set-up
    char mes[VRAM_W];
    printf("A Keyboard device (address %d) is mounted\r\n", dev_addr);
    sprintf(mes, "A Keyboard device (address %d) is mounted\r\n", dev_addr);
    gvram_strings(0, 24, mes, WDOT);
    tuh_hid_keyboard_get_report(dev_addr, &usb_keyboard_report);
}

void tuh_hid_keyboard_unmounted_cb(uint8_t dev_addr) {
    // application tear-down
    char mes[VRAM_W];
    printf("A Keyboard device (address %d) is unmounted\r\n", dev_addr);
    sprintf(mes, "A Keyboard device (address %d) is unmounted\r\n", dev_addr);
    gvram_strings(0, 24, mes, WDOT);
}

// invoked ISR context
void tuh_hid_keyboard_isr(uint8_t dev_addr, xfer_result_t event) {
    (void) dev_addr;
    (void) event;
}

#endif

#if CFG_TUH_HID_MOUSE

CFG_TUSB_MEM_SECTION static hid_mouse_report_t usb_mouse_report;

void cursor_movement(int8_t x, int8_t y, int8_t wheel) {
    //------------- X -------------//
    if (x < 0) {
        printf(ANSI_CURSOR_BACKWARD(%d), (-x)); // move left
    } else if (x > 0) {
        printf(ANSI_CURSOR_FORWARD(%d), x); // move right
    } else {}

    //------------- Y -------------//
    if (y < 0) {
        printf(ANSI_CURSOR_UP(%d), (-y)); // move up
    } else if (y > 0) {
        printf(ANSI_CURSOR_DOWN(%d), y); // move down
    } else {}

    //------------- wheel -------------//
    if (wheel < 0) {
        printf(ANSI_SCROLL_UP(%d), (-wheel)); // scroll up
    } else if (wheel > 0) {
        printf(ANSI_SCROLL_DOWN(%d), wheel); // scroll down
    } else {}
}

static inline void process_mouse_report(hid_mouse_report_t const *p_report) {
    static hid_mouse_report_t prev_report = {0};

    //------------- button state  -------------//
    uint8_t button_changed_mask = p_report->buttons ^prev_report.buttons;
    if (button_changed_mask & p_report->buttons) {
        printf(" %c%c%c ",
               p_report->buttons & MOUSE_BUTTON_LEFT ? 'L' : '-',
               p_report->buttons & MOUSE_BUTTON_MIDDLE ? 'M' : '-',
               p_report->buttons & MOUSE_BUTTON_RIGHT ? 'R' : '-');
    }

    //------------- cursor movement -------------//
    cursor_movement(p_report->x, p_report->y, p_report->wheel);
}


void tuh_hid_mouse_mounted_cb(uint8_t dev_addr) {
    // application set-up
    printf("A Mouse device (address %d) is mounted\r\n", dev_addr);
}

void tuh_hid_mouse_unmounted_cb(uint8_t dev_addr) {
    // application tear-down
    printf("A Mouse device (address %d) is unmounted\r\n", dev_addr);
}

// invoked ISR context
void tuh_hid_mouse_isr(uint8_t dev_addr, xfer_result_t event) {
    (void) dev_addr;
    (void) event;
}

#endif


void hid_task(void) {
    uint8_t const addr = 1;

#if CFG_TUH_HID_KEYBOARD
    if (tuh_hid_keyboard_is_mounted(addr)) {
        if (!tuh_hid_keyboard_is_busy(addr)) {
            process_kbd_report(&usb_keyboard_report);
            tuh_hid_keyboard_get_report(addr, &usb_keyboard_report);
        }
    }
#endif

#if CFG_TUH_HID_MOUSE
    if (tuh_hid_mouse_is_mounted(addr)) {
        if (!tuh_hid_mouse_is_busy(addr)) {
            process_mouse_report(&usb_mouse_report);
            tuh_hid_mouse_get_report(addr, &usb_mouse_report);
        }
    }
#endif
}


//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void) {
    const uint32_t interval_ms = 250;
    static uint32_t start_ms = 0;

    static bool led_state = false;

    // Blink every interval ms
    if (board_millis() - start_ms < interval_ms) return; // not enough time
    start_ms += interval_ms;

    board_led_write(led_state);
    led_state = 1 - led_state; // toggle
}

//--------------------------------------------------------------------+
// HELPER FUNCTION
//--------------------------------------------------------------------+
void print_greeting(void) {
    printf("This Host demo is configured to support:\n");
    if (CFG_TUH_HID_KEYBOARD) puts("  - HID Keyboard");
    if (CFG_TUH_HID_MOUSE) puts("  - HID Mouse");
    gvram_strings(0, 0, "This Host demo is configured to support:", WDOT);
    if (CFG_TUH_HID_KEYBOARD) gvram_strings(0, 8, "  - HID Keyboard", WDOT);
    if (CFG_TUH_HID_MOUSE) gvram_strings(0, 16, "  - HID Mouse", WDOT);
}

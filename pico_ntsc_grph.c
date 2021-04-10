/**
 * NTSC signal generation with PWM interrupt
 * ATTENTION: You should compile this source with Release option of ARM compiler.
 * Mar.24, 2021 Pa@ART modified from test_ntsc_wall_grph.c
 */


#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "font8x8_basic.h"
#include "pico_ntsc_grph.h"

static uint8_t gvram[GVRAM_W][GVRAM_H];
static uint8_t vram[VRAM_W][VRAM_H];
static int32_t count;

// to clear VRAM contents (set to 0)
void vram_clear( void ) {
    for (int i = 0; i < VRAM_W; i++) {
        for (int j = 0; j < VRAM_H; j++) {
            vram[i][j] = 0;
        }
    }
}

// to clear graphical VRAM contents (set to 0)
void gvram_clear( void ) {
    for (int i = 0; i < GVRAM_W; i++) {
        for (int j = 0; j < GVRAM_H; j++) {
            gvram[i][j] = BDOT;
        }
    }
}

// to write a value into VRAM located at (x, y)
void vram_write ( int x, int y, unsigned char value ) {
    vram[x][y] = value;
}

// to write a value into graphical VRAM located at (x, y)
void gvram_write ( int x, int y, unsigned char value ) {
    gvram[x][y] = value;
}

// put a character on VRAM
void gvram_put_char( int x, int y, char c, char col ) {
    unsigned char cp;
    for (int i = 0; i < CHAR_H; i++) {
        cp = ascii_table[c][i];
        for (int j = 0; j < CHAR_W; j++) {
            if ((x + j < GVRAM_W) && (y + i < GVRAM_H)) {
                if ((cp & (1 << j)) != 0) {
                    gvram[x + j][y + i] = col;
                } else {
                    gvram[x + j][y + i] = BDOT;
                }
            }
        }
    }
}

// put strings on VRAM
void gvram_strings( int x, int y, char *mes, char col ) {
    // if invalid (x, y), return
    if ((x < 0) || (x > GVRAM_W) || (y < 0) || (y > GVRAM_H)) {
        return;
    }
    int l = strlen(mes);
    for (int i = 0; i < l * CHAR_W; i += CHAR_W) {
        // if x position overflows, return
        if (x + i >= GVRAM_W) {
            return;
        // else put a character at the position
        } else {
            gvram_put_char(x + i, y, mes[i / CHAR_W], col);
        }
    }
    return;
}

// initialize video and LED GPIO
void init_video_and_led_GPIO( ) {
    // initialize GPIO14 and GPIO15 for masked output
    gpio_init(GP14);
    gpio_init(GP15);
    gpio_init_mask(M14 | M15);
    gpio_set_dir(GP14, GPIO_OUT);
    gpio_set_dir(GP15, GPIO_OUT);
    // initialize LED GPIO
    gpio_init(LED);
    gpio_init_mask(MLED);
    gpio_set_dir(LED, GPIO_OUT);
}

// to generate horizontal sync siganl
void hsync( void ) {
    SYNC;
    sleep_us(5);
    BLACK;
    sleep_us(7);
}

// to generate vertical sync siganl
void vsync ( void ) {
    SYNC;
    sleep_us(10);
    sleep_us(5);
    sleep_us(10);
    BLACK;
    sleep_us(5);

    SYNC;
    sleep_us(10);
    sleep_us(5);
    sleep_us(10);
    BLACK;
    sleep_us(5);
}

// handler for holizontal line processing
void horizontal_line( ) {
    // Clear the interrupt flag that brought us here
    pwm_clear_irq(pwm_gpio_to_slice_num(GPPWM));

    // vertical synchronization duration
    if (count >= 3 && count <= 5) {
        vsync();    // vertical SYNC

    // VRAM drawing area
    } else if (count >= V_BASE && count < V_BASE + VRAM_H * CHAR_H) {
        hsync();
        // left blank??
        BLACK;
        sleep_us(0);    // should be tuned

        int y = count - V_BASE;
        for (int x = 0; x < GVRAM_W; x++) {
            int c = gvram[x][y];
            switch (c) {
                case BDOT:  // black
                BLACK;
                break;
                case WDOT:  // white
                WHITE;
                break;
                case GDOT:  // gray
                GRAY;
                break;
                //case ZDOT:  // dummy
                //break;
                default:
                BLACK;
            }
        }
        // right blank??
        BLACK;
        sleep_us(0);
    } else {
        hsync();
        BLACK;
    }
    // count up scan line 
    count++;
    // if scan line reach to max
    if (count > 262) {
        count = 1;
    }
    return;
}

// initialize and start PWM interrupt by 64us period
void enable_PWM_interrupt(  ) {

    // GPPWM pin is the PWM output
    gpio_set_function(GPPWM, GPIO_FUNC_PWM);
    // Figure out which slice we just connected to the GPPWM pin
    uint slice_num = pwm_gpio_to_slice_num(GPPWM);

    // Mask our slice's IRQ output into the PWM block's single interrupt line,
    // and register our interrupt handler
    pwm_clear_irq(slice_num);
    pwm_set_irq_enabled(slice_num, true);
    irq_set_priority(PWM_IRQ_WRAP, 0xC0);   // somehow this is needed if you compile with release option
    irq_set_exclusive_handler(PWM_IRQ_WRAP, horizontal_line);
    irq_set_enabled(PWM_IRQ_WRAP, true); 

    // Set counter wrap value to generate PWM interrupt by this value
    pwm_set_wrap(slice_num, 7999);
    // Load the configuration into our PWM slice, and set it running.
    pwm_set_enabled(slice_num, true);
}
/**
 * Header file for NTSC signal generation with PWM interrupt
 * ATTENTION: You should compile this source with Release option of ARM compiler.
 * Mar.24, 2021 Pa@ART modified from test_ntsc_wall_grph.c
 */


#ifndef __PICO_NTSC_GRPH__
#define __PICO_NTSC_GRPH__

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"

#define LED     25      // GPIO connected LED on the board
#define MLED    (1 << LED)
#define LEDON   gpio_put_masked(MLED, MLED)
#define LEDOFF  gpio_put_masked(MLED, 0)
#define GPPWM   2       // GPIO2 is PWM output
#define GP14    14      // GPIO14 connected to RCA+ pin via 330 ohm
#define GP15    15      // GPIO15 connected to RCA+ pin via 1k ohm
#define M14     (1 << GP14) // bit mask for GPIO14
#define M15     (1 << GP15) // bit mask for GPIO15
#define SYNC    gpio_put_masked(M14 | M15, 0)             // GPIO14='L' and GPIO15='L'
#define WHITE   gpio_put_masked(M14 | M15, M14 | M15)     // GPIO14='H' and GPIO15='H'
#define BLACK   gpio_put_masked(M14 | M15, M15)           // GPIO14='L' and GPIO15='H'
#define GRAY    gpio_put_masked(M14 | M15, M14)           // GPIO14='H' and GPIO15='L'
#define VRAM_W  50      // width size of VRAM
#define VRAM_H  25      // height size of VRAM
#define GVRAM_W (VRAM_W * CHAR_W)   // width size of graphic VRAM
#define GVRAM_H (VRAM_H * CHAR_H)   // height size of graphic VRAM
#define V_BASE  40      // horizontal line number to start displaying VRAM
#define BDOT    0       // black dot
#define WDOT    1       // white dot
#define GDOT    2       // gray dot
#define ZDOT    3       // dummy dot

// to clear VRAM contents (set to 0)
void vram_clear( );
// to clear graphical VRAM contents (set to 0)
void gvram_clear( );
// to write a value into VRAM located at (x, y)
void vram_write ( int x, int y, unsigned char value );
// to write a value into graphical VRAM located at (x, y)
void gvram_write ( int x, int y, unsigned char value );
// put a character on VRAM
void gvram_put_char( int x, int y, char c, char col );
// put strings on VRAM
void gvram_strings( int x, int y, char *mes, char col );
// initialize video and LED GPIO
void init_video_and_led_GPIO( );
// to generate horizontal sync siganl
void hsync( );
// to generate vertical sync siganl
void vsync ( );
// handler for holizontal line processing
void horizontal_line( );
// initialize and start PWM interrupt by 64us period
void enable_PWM_interrupt( );

#endif
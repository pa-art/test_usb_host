/**
 * Header file for mini game pad driver
 * ATTENTION: You should compile this source with Release option of ARM compiler.
 * Mar.24, 2021 Pa@ART modified from test_ntsc_wall_grph.c
 */


#ifndef __MINI_GAMEPAD__
#define __MINI_GAMEPAD__

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define RKEYGP  11      // Right key GP11
#define UKEYGP  10      // Up key GP10
#define DKEYGP  9       // Down key GP9
#define LKEYGP  8       // Left key GP8
#define AKEYGP  7       // A key GP7
#define BKEYGP  6       // B key GP6
#define RKEY    (1 << RKEYGP)
#define UKEY    (1 << UKEYGP)
#define LKEY    (1 << LKEYGP)
#define DKEY    (1 << DKEYGP)
#define AKEY    (1 << AKEYGP)
#define BKEY    (1 << BKEYGP)

// initialize Key GPIO setting
void init_key_GPIO( );
// scan keys
uint32_t key_scan( );
// initialize random seed by ADC data
void init_random( );

#endif
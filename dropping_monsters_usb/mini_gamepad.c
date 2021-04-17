/**
 * Mini game pad driver
 * ATTENTION: You should compile this source with Release option of ARM compiler.
 * Mar.24, 2021 Pa@ART modified from test_ntsc_wall_grph.c
 */


#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "mini_gamepad.h"

// initialize Key GPIO setting
void init_key_GPIO( ) {
    // GPIO init
    gpio_init(RKEYGP);
    gpio_init(UKEYGP);
    gpio_init(DKEYGP);
    gpio_init(LKEYGP);
    gpio_init(AKEYGP);
    gpio_init(BKEYGP);
    // GPIO direction
    gpio_set_dir(RKEYGP, GPIO_IN);
    gpio_set_dir(UKEYGP, GPIO_IN);
    gpio_set_dir(DKEYGP, GPIO_IN);
    gpio_set_dir(LKEYGP, GPIO_IN);
    gpio_set_dir(AKEYGP, GPIO_IN);
    gpio_set_dir(BKEYGP, GPIO_IN);
    // GPIO pullup
    gpio_pull_up(RKEYGP);
    gpio_pull_up(UKEYGP);
    gpio_pull_up(DKEYGP);
    gpio_pull_up(LKEYGP);
    gpio_pull_up(AKEYGP);
    gpio_pull_up(BKEYGP);
}

// scan keys
uint32_t key_scan( ) {

    uint32_t result = 0x0;

    if (gpio_get(RKEYGP) == 0) result |= RKEY;
    if (gpio_get(UKEYGP) == 0) result |= UKEY;
    if (gpio_get(DKEYGP) == 0) result |= DKEY;
    if (gpio_get(LKEYGP) == 0) result |= LKEY;
    if (gpio_get(AKEYGP) == 0) result |= AKEY;
    if (gpio_get(BKEYGP) == 0) result |= BKEY;

    return result;
}

// initialize random seed by ADC data
void init_random( ) {
    // init ADC
    adc_init();
    // enable temperature sensor
    adc_set_temp_sensor_enabled(true);

    int seed_val = 0;
    for (int i = 0; i < 5; i++) {
        adc_select_input(i);
        seed_val += adc_read();
    }
    srand(seed_val);
}

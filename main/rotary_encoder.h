/**
 * Rotary encoder — polling driver (ESP-IDF)
 * A=IO27, B=IO28, Push=IO5
 */
#pragma once

#include <stdbool.h>

void encoder_init(void);
int  encoder_poll(void);      /* returns -1, 0, or +1 */
bool encoder_button_pressed(void);  /* push button state */

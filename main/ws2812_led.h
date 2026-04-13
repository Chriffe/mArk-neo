/**
 * WS2812 LED progress indicator — 4 LEDs on IO2 via RMT
 */
#pragma once

#include <stdint.h>

void ws2812_init(void);
void ws2812_update_progress(int completed, int total);
void ws2812_set_all(uint8_t r, uint8_t g, uint8_t b);
void ws2812_off(void);

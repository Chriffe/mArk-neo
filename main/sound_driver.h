/**
 * Sound driver — I2S audio for CrowPanel Advanced (ESP-IDF)
 * Uses SPK-R and SPK-L ports with built-in amplifier
 */
#pragma once

void sound_init(void);
void sound_task_complete(void);
void sound_day_complete(void);

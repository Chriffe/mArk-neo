/**
 * WS2812 LED driver — 4 LEDs on IO2 using RMT (ESP-IDF)
 */
#include "ws2812_led.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "driver/rmt_tx.h"
#include "esp_log.h"
#include "led_strip_encoder.h"

static const char *TAG = "WS2812";

#define WS2812_GPIO     2
#define WS2812_NUM_LEDS 32
#define RMT_RESOLUTION  10000000  /* 10 MHz — 1 tick = 0.1µs */

static rmt_channel_handle_t led_chan = NULL;
static rmt_encoder_handle_t led_encoder = NULL;
static uint8_t led_pixels[WS2812_NUM_LEDS * 3];  /* GRB format */

/* Green for completed tasks */
#define PROGRESS_R 5
#define PROGRESS_G 40
#define PROGRESS_B 5

/* Unlit LEDs: fully off */
#define OFF_R 0
#define OFF_G 0
#define OFF_B 0

void ws2812_init(void) {
    rmt_tx_channel_config_t tx_cfg = {
        .gpio_num = WS2812_GPIO,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = RMT_RESOLUTION,
        .mem_block_symbols = 64,
        .trans_queue_depth = 4,
    };

    esp_err_t err = rmt_new_tx_channel(&tx_cfg, &led_chan);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "RMT TX channel init failed: %s", esp_err_to_name(err));
        return;
    }
    ESP_LOGW(TAG, "RMT TX channel created OK on GPIO%d", WS2812_GPIO);

    led_strip_encoder_config_t enc_cfg = {
        .resolution = RMT_RESOLUTION,
    };
    err = rmt_new_led_strip_encoder(&enc_cfg, &led_encoder);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "LED encoder init failed");
        return;
    }

    rmt_enable(led_chan);

    /* Start dark — progress bar will be set after calendar fetch */
    ws2812_off();

    ESP_LOGI(TAG, "Init OK: %d LEDs on IO%d", WS2812_NUM_LEDS, WS2812_GPIO);
}

static void ws2812_flush(void) {
    if (!led_chan || !led_encoder) {
        ESP_LOGE(TAG, "flush skipped: chan=%p enc=%p", led_chan, led_encoder);
        return;
    }

    rmt_transmit_config_t tx_config = { .loop_count = 0 };
    esp_err_t err = rmt_transmit(led_chan, led_encoder, led_pixels, sizeof(led_pixels), &tx_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "rmt_transmit FAILED: %s", esp_err_to_name(err));
        return;
    }
    err = rmt_tx_wait_all_done(led_chan, pdMS_TO_TICKS(1000));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "rmt_tx_wait FAILED: %s", esp_err_to_name(err));
    }
    ESP_LOGI(TAG, "flush OK (%d bytes)", (int)sizeof(led_pixels));
}

void ws2812_set_all(uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < WS2812_NUM_LEDS; i++) {
        led_pixels[i * 3 + 0] = g;  /* GRB order */
        led_pixels[i * 3 + 1] = r;
        led_pixels[i * 3 + 2] = b;
    }
    ws2812_flush();
}

void ws2812_off(void) {
    memset(led_pixels, 0, sizeof(led_pixels));
    ws2812_flush();
}

void ws2812_update_progress(int completed, int total) {
    if (total <= 0) {
        ws2812_off();
        return;
    }

    /* How many LEDs should be lit */
    int lit = (completed * WS2812_NUM_LEDS + total - 1) / total;
    if (completed == 0) lit = 0;
    if (completed >= total) lit = WS2812_NUM_LEDS;

    for (int i = 0; i < WS2812_NUM_LEDS; i++) {
        if (i < lit) {
            led_pixels[i * 3 + 0] = PROGRESS_G;
            led_pixels[i * 3 + 1] = PROGRESS_R;
            led_pixels[i * 3 + 2] = PROGRESS_B;
        } else {
            led_pixels[i * 3 + 0] = OFF_G;
            led_pixels[i * 3 + 1] = OFF_R;
            led_pixels[i * 3 + 2] = OFF_B;
        }
    }
    ws2812_flush();
}

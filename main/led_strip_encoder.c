/**
 * WS2812 RMT encoder — based on ESP-IDF led_strip example
 */
#include "led_strip_encoder.h"
#include "esp_check.h"

typedef struct {
    rmt_encoder_t base;
    rmt_encoder_t *bytes_encoder;
    rmt_encoder_t *copy_encoder;
    int state;
    rmt_symbol_word_t reset_code;
} led_strip_encoder_t;

static size_t led_strip_encode(rmt_encoder_t *encoder, rmt_channel_handle_t channel,
                                const void *primary_data, size_t data_size,
                                rmt_encode_state_t *ret_state) {
    led_strip_encoder_t *led_encoder = __containerof(encoder, led_strip_encoder_t, base);
    rmt_encode_state_t session_state = RMT_ENCODING_RESET;
    size_t encoded_symbols = 0;

    switch (led_encoder->state) {
    case 0:  /* send pixel data */
        encoded_symbols += led_encoder->bytes_encoder->encode(
            led_encoder->bytes_encoder, channel, primary_data, data_size, &session_state);
        if (session_state & RMT_ENCODING_COMPLETE) {
            led_encoder->state = 1;
        }
        if (session_state & RMT_ENCODING_MEM_FULL) {
            *ret_state = (rmt_encode_state_t)(session_state & ~RMT_ENCODING_COMPLETE);
            return encoded_symbols;
        }
        /* fall through */
    case 1:  /* send reset code */
        encoded_symbols += led_encoder->copy_encoder->encode(
            led_encoder->copy_encoder, channel, &led_encoder->reset_code,
            sizeof(led_encoder->reset_code), &session_state);
        if (session_state & RMT_ENCODING_COMPLETE) {
            led_encoder->state = RMT_ENCODING_RESET;
            *ret_state = RMT_ENCODING_COMPLETE;
        }
        if (session_state & RMT_ENCODING_MEM_FULL) {
            *ret_state = (rmt_encode_state_t)(session_state & ~RMT_ENCODING_COMPLETE);
        }
        return encoded_symbols;
    }
    *ret_state = RMT_ENCODING_RESET;
    return 0;
}

static esp_err_t led_strip_del(rmt_encoder_t *encoder) {
    led_strip_encoder_t *led_encoder = __containerof(encoder, led_strip_encoder_t, base);
    rmt_del_encoder(led_encoder->bytes_encoder);
    rmt_del_encoder(led_encoder->copy_encoder);
    free(led_encoder);
    return ESP_OK;
}

static esp_err_t led_strip_reset(rmt_encoder_t *encoder) {
    led_strip_encoder_t *led_encoder = __containerof(encoder, led_strip_encoder_t, base);
    rmt_encoder_reset(led_encoder->bytes_encoder);
    rmt_encoder_reset(led_encoder->copy_encoder);
    led_encoder->state = RMT_ENCODING_RESET;
    return ESP_OK;
}

esp_err_t rmt_new_led_strip_encoder(const led_strip_encoder_config_t *config,
                                     rmt_encoder_handle_t *ret_encoder) {
    led_strip_encoder_t *led_encoder = calloc(1, sizeof(led_strip_encoder_t));
    ESP_RETURN_ON_FALSE(led_encoder, ESP_ERR_NO_MEM, "WS2812", "alloc failed");

    led_encoder->base.encode = led_strip_encode;
    led_encoder->base.del = led_strip_del;
    led_encoder->base.reset = led_strip_reset;

    /* WS2812 timing: T0H=0.3µs, T0L=0.9µs, T1H=0.9µs, T1L=0.3µs */
    rmt_bytes_encoder_config_t bytes_cfg = {
        .bit0 = { .duration0 = 0.3 * config->resolution / 1000000,
                   .level0 = 1,
                   .duration1 = 0.9 * config->resolution / 1000000,
                   .level1 = 0 },
        .bit1 = { .duration0 = 0.9 * config->resolution / 1000000,
                   .level0 = 1,
                   .duration1 = 0.3 * config->resolution / 1000000,
                   .level1 = 0 },
        .flags.msb_first = 1,
    };
    ESP_RETURN_ON_ERROR(rmt_new_bytes_encoder(&bytes_cfg, &led_encoder->bytes_encoder),
                        "WS2812", "bytes encoder");

    rmt_copy_encoder_config_t copy_cfg = {};
    ESP_RETURN_ON_ERROR(rmt_new_copy_encoder(&copy_cfg, &led_encoder->copy_encoder),
                        "WS2812", "copy encoder");

    /* Reset code: 50µs low */
    led_encoder->reset_code = (rmt_symbol_word_t){
        .duration0 = config->resolution / 1000000 * 50 / 2,
        .level0 = 0,
        .duration1 = config->resolution / 1000000 * 50 / 2,
        .level1 = 0,
    };

    *ret_encoder = &led_encoder->base;
    return ESP_OK;
}

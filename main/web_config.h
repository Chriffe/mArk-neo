/**
 * Web Configuration Server — serves a mobile-friendly page
 * for editing calendar sources via phone browser.
 */
#pragma once

#include <stdbool.h>

/**
 * Start the web config server on port 80.
 * Call after WiFi is connected.
 */
void web_config_start(void);

/**
 * Get the device's IP address as a string (e.g. "192.168.1.42").
 * Returns empty string if not connected.
 */
const char *web_config_get_ip(void);

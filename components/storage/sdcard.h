#pragma once

#include "esp_err.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the SPI microSD interface and mount FATFS on /sdcard.
 *
 * Safe to call multiple times; the driver will only be initialized once.
 */
esp_err_t sdcard_init(void);

/**
 * @brief Return true if the card is currently mounted.
 */
bool sdcard_is_mounted(void);

/**
 * @brief Simple smoke test that writes and reads a file on /sdcard.
 */
esp_err_t sdcard_test_file(void);

#ifdef __cplusplus
}
#endif

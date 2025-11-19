#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t rs485_init(void);
esp_err_t rs485_write(const uint8_t *data, size_t len, TickType_t timeout);
esp_err_t rs485_read(uint8_t *data, size_t max_len, size_t *out_len, TickType_t timeout);

#ifdef __cplusplus
}
#endif


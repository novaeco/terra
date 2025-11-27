#pragma once

#include "esp_err.h"
#include "lvgl.h"

/**
 * @brief Start the LVGL runtime (tick source + handler task).
 *
 * The tick uses a single esp_timer periodic callback and the handler runs in a
 * dedicated FreeRTOS task. Subsequent calls are ignored once started.
 */
esp_err_t lvgl_runtime_start(lv_display_t *disp);

/**
 * @brief Wait for the LVGL handler task to start.
 *
 * @param timeout_ms Maximum time to wait in milliseconds.
 * @return true if the handler task started within the timeout, false otherwise.
 */
bool lvgl_runtime_wait_started(uint32_t timeout_ms);

/**
 * @brief Return the cumulative tick callback count (incremented once per tick).
 */
uint32_t lvgl_tick_alive_count(void);


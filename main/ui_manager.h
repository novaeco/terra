#pragma once

#include <stdbool.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t ui_manager_init(void);
void ui_manager_set_degraded(bool degraded);

#ifdef __cplusplus
}
#endif

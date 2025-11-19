#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void cs8501_init(void);
float cs8501_get_battery_voltage(void);
bool cs8501_is_charging(void);

#ifdef __cplusplus
}
#endif


#include "cs8501.h"

#include "esp_log.h"

static const char *TAG = "CS8501";

void cs8501_init(void)
{
    ESP_LOGI(TAG, "CS8501 power controller stub initialized");
}

float cs8501_get_battery_voltage(void)
{
    return 4.0f;  // Stub value, replace with ADC/driver reading
}

bool cs8501_is_charging(void)
{
    return false;  // Stub state until CS8501 monitoring is implemented
}

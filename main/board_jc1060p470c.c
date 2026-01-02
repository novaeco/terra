#include "board_jc1060p470c.h"
#include "esp_log.h"
#include "driver/ledc.h"

static const char *TAG = "board";

esp_err_t board_init_pins(void)
{
    gpio_config_t out_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config_t in_conf = out_conf;
    out_conf.pin_bit_mask = (1ULL << BOARD_PIN_LCD_RST) | (1ULL << BOARD_PIN_LCD_STBYB) |
                            (1ULL << BOARD_PIN_BL_EN) | (1ULL << BOARD_PIN_BL_PWM);
    ESP_ERROR_CHECK(gpio_config(&out_conf));

    in_conf.mode = GPIO_MODE_INPUT;
    in_conf.pin_bit_mask = (1ULL << BOARD_PIN_TOUCH_INT);
    ESP_ERROR_CHECK(gpio_config(&in_conf));

    // I2C pins will be configured by driver

    // Default states
    gpio_set_level(BOARD_PIN_LCD_STBYB, 1);
    gpio_set_level(BOARD_PIN_LCD_RST, 0);
    gpio_set_level(BOARD_PIN_BL_EN, 0);
    gpio_set_level(BOARD_PIN_BL_PWM, 0);
    return ESP_OK;
}

esp_err_t board_backlight_on(void)
{
    ledc_timer_config_t timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 20000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer));
    ledc_channel_config_t ch = {
        .gpio_num = BOARD_PIN_BL_PWM,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 512,
        .hpoint = 0,
        .intr_type = LEDC_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ch));
    ESP_LOGI(TAG, "backlight: enable pin %d, pwm %d", BOARD_PIN_BL_EN, BOARD_PIN_BL_PWM);
    gpio_set_level(BOARD_PIN_BL_EN, 1);
    return ESP_OK;
}

esp_err_t board_backlight_off(void)
{
    gpio_set_level(BOARD_PIN_BL_EN, 0);
    ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    return ESP_OK;
}

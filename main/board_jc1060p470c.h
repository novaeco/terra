#pragma once
#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"

#define BOARD_LCD_H_RES 1024
#define BOARD_LCD_V_RES 600

// Pins depuis schéma JC1060_schematic-1 (bloc DSI_LCD, Capacitive touch, Blacklighting)
#define BOARD_PIN_LCD_RST      GPIO_NUM_14   // LCD_RST (schéma DSI_LCD)
#define BOARD_PIN_LCD_STBYB    GPIO_NUM_15   // STBYB tiré haut, option pour contrôle
#define BOARD_PIN_BL_EN        GPIO_NUM_2    // BLK_EN
#define BOARD_PIN_BL_PWM       GPIO_NUM_1    // BL_PWM

#define BOARD_PIN_TOUCH_SDA    GPIO_NUM_9    // IO9
#define BOARD_PIN_TOUCH_SCL    GPIO_NUM_8    // IO8
#define BOARD_PIN_TOUCH_RST    GPIO_NUM_10   // IO10
#define BOARD_PIN_TOUCH_INT    GPIO_NUM_11   // IO11

// UART0 console (bloc JC-ESP32P4-M3, HW_NOTES)
#define BOARD_PIN_UART0_TX     GPIO_NUM_43
#define BOARD_PIN_UART0_RX     GPIO_NUM_44
#define BOARD_PIN_BOOT_MODE    GPIO_NUM_9    // partagé avec SDA tactile, strap bas pour boot normal

esp_err_t board_init_pins(void);
esp_err_t board_backlight_on(void);
esp_err_t board_backlight_off(void);

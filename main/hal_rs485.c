#include "hal_rs485.h"
#include "driver/uart.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "hal_rs485";
static bool rs485_ready = false;

esp_err_t hal_rs485_init(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    ESP_ERROR_CHECK(uart_param_config(RS485_UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(RS485_UART_PORT, RS485_TXD, RS485_RXD, RS485_RTS, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(RS485_UART_PORT, 256, 256, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_set_mode(RS485_UART_PORT, UART_MODE_RS485_HALF_DUPLEX));
    rs485_ready = true;
    return ESP_OK;
}

esp_err_t hal_rs485_send(const char *data)
{
    if (!rs485_ready)
    {
        return ESP_ERR_INVALID_STATE;
    }
    int len = strlen(data);
    int written = uart_write_bytes(RS485_UART_PORT, data, len);
    return (written == len) ? ESP_OK : ESP_FAIL;
}

esp_err_t hal_rs485_receive(char *buffer, size_t max_len, TickType_t ticks_to_wait)
{
    if (!rs485_ready)
    {
        return ESP_ERR_INVALID_STATE;
    }
    int len = uart_read_bytes(RS485_UART_PORT, (uint8_t *)buffer, max_len - 1, ticks_to_wait);
    if (len < 0)
    {
        return ESP_FAIL;
    }
    buffer[len] = '\0';
    return ESP_OK;
}

bool hal_rs485_is_ready(void)
{
    return rs485_ready;
}

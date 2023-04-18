#include "mhz19e.hpp"

esp_err_t mhz19e::get_reading(uint16_t *co2_out)
{
    return ESP_OK;
}

esp_err_t mhz19e::get_reading(uint16_t *co2_out, uint16_t *temp_out)
{
    return ESP_OK;
}

esp_err_t mhz19e::set_auto_calib(bool enable)
{
    return ESP_OK;
}

esp_err_t mhz19e::calib_zero_point()
{
    return ESP_OK;
}

esp_err_t mhz19e::calib_span_point()
{
    return ESP_OK;
}

esp_err_t mhz19e::set_range(uint16_t range)
{
    return ESP_OK;
}

esp_err_t mhz19e::init(gpio_num_t tx, gpio_num_t rx, uart_port_t _port, size_t rx_buf_size)
{
    port = _port;
    uart_config_t uart_config = {};
    uart_config.baud_rate = 9600;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_config.source_clk = UART_SCLK_APB;

    auto ret = uart_driver_install(port, (int)rx_buf_size, 0, 20, &uart_evt_queue, 0);
    ret = ret ?: uart_param_config(port, &uart_config);
    ret = ret ?: uart_set_pin(port, tx, rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    ret = ret ?: uart_enable_pattern_det_baud_intr(port, (char)0xff, 1, 9, 0, 0);
    ret = ret ?: uart_pattern_queue_reset(port, 20);

    if (ret != ESP_OK) {
        return ret;
    } else {
        if (xTaskCreate(uart_event_task, "mhz19e_uart", 8192, this, tskIDLE_PRIORITY + 3, nullptr) != pdPASS) {
            return ESP_ERR_NO_MEM;
        }
    }

    return ret;
}

void mhz19e::uart_event_task(void *ctx)
{

}

#include <esp_log.h>

#include "mhz19e.hpp"

esp_err_t mhz19e::get_reading(uint16_t *co2_out, uint32_t timeout_ticks) const
{
    if (co2_out == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    auto ret = uart_flush(port);

    auto tx_len = uart_write_bytes(port, CMD_READ_GAS_CONCENTRATION, sizeof(CMD_READ_GAS_CONCENTRATION));
    if (tx_len < 0) {
        ESP_LOGE(TAG, "Get reading: send fail");
        return ret;
    }

    ret = uart_wait_tx_done(port, timeout_ticks);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Get reading: Tx wait timeout");
        return ret;
    }

    uint8_t rx_buf[9] = { 0 };
    auto read_len = uart_read_bytes(port, rx_buf, sizeof(rx_buf), timeout_ticks);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, rx_buf, sizeof(rx_buf), ESP_LOG_VERBOSE);
    if (read_len < 9) {
        ESP_LOGE(TAG, "Get reading: insufficient data, got %u", read_len);
        return ESP_ERR_INVALID_SIZE;
    }

    uint8_t checksum = calc_checksum(rx_buf, sizeof(rx_buf));
    if (rx_buf[8] != checksum) {
        ESP_LOGE(TAG, "Get reading: invalid checksum: 0x%02x vs 0x%02x", rx_buf[8], checksum);
        return ESP_ERR_INVALID_CRC;
    }

    *co2_out = rx_buf[2] * 256 + rx_buf[3];
    return ESP_OK;
}

esp_err_t mhz19e::calib_zero_point(uint32_t timeout_ticks) const
{
    auto ret = uart_flush(port);

    auto tx_len = uart_write_bytes(port, CMD_CALIB_ZERO_POINT, sizeof(CMD_CALIB_ZERO_POINT));
    if (tx_len < 0) {
        ESP_LOGE(TAG, "Get reading: send fail");
        return ret;
    }

    ret = ret ?: uart_wait_tx_done(port, timeout_ticks);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Calib zero point: send fail");
        return ret;
    }

    return ESP_OK;
}

esp_err_t mhz19e::calib_span_point(uint32_t timeout_ticks) const
{
    auto ret = uart_flush(port);

    auto tx_len = uart_write_bytes(port, CMD_CALIB_SPAN_POINT, sizeof(CMD_CALIB_SPAN_POINT));
    if (tx_len < 0) {
        ESP_LOGE(TAG, "Get reading: send fail");
        return ret;
    }

    ret = ret ?: uart_wait_tx_done(port, timeout_ticks);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Calib span point: send fail");
        return ret;
    }

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
    uart_config.source_clk = UART_SCLK_DEFAULT;

    gpio_reset_pin(tx);
    gpio_reset_pin(rx);
    auto ret = uart_driver_install(port, (int)rx_buf_size, 0, 0, nullptr, 0);
    ret = ret ?: uart_param_config(port, &uart_config);
    ret = ret ?: uart_set_pin(port, tx, rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    return ret;
}

uint8_t mhz19e::calc_checksum(const uint8_t *packet, size_t len)
{
    if (packet == nullptr) {
        return 0;
    }

    uint8_t checksum = 0;
    for (size_t idx = 1; idx < (len - 1); idx += 1) {
        checksum += packet[idx];
    }

    checksum = 0xff - checksum;
    checksum += 1;
    return checksum;
}

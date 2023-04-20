#pragma once

#include <cstdint>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include <esp_err.h>
#include <driver/gpio.h>
#include <driver/uart.h>

namespace mhz19_def
{
    enum cmd : uint8_t {
        AUTO_CALIBRATION = 0x79,
        READ_CO2_TEMP = 0x85,
        READ_CO2_ONLY = 0x86,
        CALIB_ZERO_POINT = 0x87,
        CALIB_SPAN_POINT = 0x88,
        DETECT_RANGE = 0x99,
    };
}

class mhz19e
{
public:
    static mhz19e *instance()
    {
        static mhz19e _instance;
        return &_instance;
    }

    void operator=(mhz19e const &) = delete;
    mhz19e(mhz19e const &) = delete;

private:
    mhz19e() = default;

public:
    esp_err_t init(gpio_num_t tx, gpio_num_t rx, uart_port_t _port = UART_NUM_1, size_t rx_buf_size = 512);
    esp_err_t get_reading(uint16_t *co2_out, uint32_t timeout_ticks) const;
    [[nodiscard]] esp_err_t calib_zero_point(uint32_t timeout_ticks) const;
    [[nodiscard]] esp_err_t calib_span_point(uint32_t timeout_ticks) const;

private:
    static uint8_t calc_checksum(const uint8_t *packet);

private:
    QueueHandle_t uart_evt_queue = nullptr;
    uart_port_t port = UART_NUM_1;

    static const constexpr char TAG[] = "mhz19e";
    static const constexpr uint8_t CMD_READ_GAS_CONCENTRATION[] = { 0xff, 0x01, 0x86, 0, 0, 0, 0, 0, 0x79 };
    static const constexpr uint8_t CMD_CALIB_ZERO_POINT[] = { 0xff, 0x01, 0x87, 0, 0, 0, 0, 0, 0x78 };
    static const constexpr uint8_t CMD_CALIB_SPAN_POINT[] = { 0xff, 0x01, 0x88, 0x07, 0xd0, 0, 0, 0, 0xa0 };
};

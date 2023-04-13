#pragma once

#include <cstdint>
#include <esp_err.h>
#include <driver/gpio.h>

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
    esp_err_t init(gpio_num_t tx, gpio_num_t rx);
    esp_err_t get_reading(uint16_t *co2_out);
    esp_err_t get_reading(uint16_t *co2_out, uint16_t *temp_out);
    esp_err_t set_auto_calib(bool enable);
    esp_err_t calib_zero_point();
    esp_err_t calib_span_point();
    esp_err_t set_range(uint16_t range);

private:

};

#pragma once

#include <cstdint>
#include <hal/gpio_types.h>
#include <driver/i2c.h>
#include "bme68x/bme68x_defs.h"

namespace bme680_def
{
    enum mode : uint8_t {
        SLEEP       = BME68X_SLEEP_MODE,
        FORCED      = BME68X_FORCED_MODE,
        PARALLEL    = BME68X_PARALLEL_MODE,
        SEQUENTIAL  = BME68X_SEQUENTIAL_MODE,
    };

    enum standby_time : uint8_t {
        ODR_0_59_MS = BME68X_ODR_0_59_MS,
        ODR_62_5_MS = BME68X_ODR_62_5_MS,
        ODR_125_MS  = BME68X_ODR_125_MS,
        ODR_250_MS  = BME68X_ODR_250_MS,
        ODR_500_MS  = BME68X_ODR_500_MS,
        ODR_1000_S  = BME68X_ODR_1000_MS,
        ODR_10_MS   = BME68X_ODR_10_MS,
        ODR_20_MS   = BME68X_ODR_20_MS,
    };

    enum oversampling : uint8_t {
        OS_NONE = BME68X_OS_NONE,
        OS_1X   = BME68X_OS_1X,
        OS_2X   = BME68X_OS_2X,
        OS_4X   = BME68X_OS_4X,
        OS_8X   = BME68X_OS_8X,
        OS_16X  = BME68X_OS_16X,
    };

    enum filter : uint8_t {
        FILTER_OFF      = BME68X_FILTER_OFF,
        FILTER_SIZE_1   = BME68X_FILTER_SIZE_1,
        FILTER_SIZE_3   = BME68X_FILTER_SIZE_3,
        FILTER_SIZE_7   = BME68X_FILTER_SIZE_7,
        FILTER_SIZE_15  = BME68X_FILTER_SIZE_15,
        FILTER_SIZE_31  = BME68X_FILTER_SIZE_31,
        FILTER_SIZE_63  = BME68X_FILTER_SIZE_63,
        FILTER_SIZE_127 = BME68X_FILTER_SIZE_127,
    };
}

class bme680
{
public:
    static bme680 *instance()
    {
        static bme680 _instance;
        return &_instance;
    }

    void operator=(bme680 const &) = delete;
    bme680(bme680 const &) = delete;

private:
    bme680() = default;

public:
    esp_err_t init(gpio_num_t scl, gpio_num_t sda, uint8_t _addr = 0x76, i2c_port_t _port = I2C_NUM_0);
    esp_err_t set_config(bme680_def::mode mode, bme68x_conf *config, bme68x_heatr_conf *heater_config);
    esp_err_t set_operation_mode(bme680_def::mode mode);
    esp_err_t get_reading_forced(bme68x_data *out);
    esp_err_t soft_reset();

private:
    static BME68X_INTF_RET_TYPE i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr);
    static BME68X_INTF_RET_TYPE i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr);
    static void sensor_delay_us(uint32_t period, void *intf_ptr);

private:
    uint8_t bme_addr = 0x76;
    i2c_port_t port = I2C_NUM_0;
    bme68x_dev bme_dev = {};
    bme68x_conf bme_config = {};
    bme68x_heatr_conf heater_config = {};
    static const constexpr char TAG[] = "bme680";
};

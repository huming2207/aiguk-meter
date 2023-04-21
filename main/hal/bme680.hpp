#pragma once

#include <cstdint>
#include <hal/gpio_types.h>
#include <driver/i2c.h>
#include "bme68x/bme68x_defs.h"


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
    esp_err_t init(gpio_num_t scl, gpio_num_t sda, i2c_port_t _port = I2C_NUM_0);

private:
    static BME68X_INTF_RET_TYPE i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr);
    static BME68X_INTF_RET_TYPE i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr);
    static void sensor_delay_us(uint32_t period, void *intf_ptr);

private:
    i2c_port_t port = I2C_NUM_0;
    bme68x_dev bme_dev = {};
    static const constexpr char TAG[] = "bme680";
};

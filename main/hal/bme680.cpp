#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_rom_sys.h>
#include <esp_log.h>

#include "bme680.hpp"
#include "bme68x/bme68x.h"

BME68X_INTF_RET_TYPE bme680::i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    return 0;
}

BME68X_INTF_RET_TYPE bme680::i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    return 0;
}

void bme680::sensor_delay_us(uint32_t period, void *intf_ptr)
{
    if (period > 1000) {
        vTaskDelay(pdMS_TO_TICKS(period / 1000));
    } else {
        esp_rom_delay_us(period);
    }
}

esp_err_t bme680::init(gpio_num_t scl, gpio_num_t sda, i2c_port_t _port)
{
    port = _port;

    i2c_config_t i2c_cfg = {};
    i2c_cfg.mode = I2C_MODE_MASTER;
    i2c_cfg.clk_flags = 0;
    i2c_cfg.scl_io_num = scl;
    i2c_cfg.scl_pullup_en = true;
    i2c_cfg.sda_io_num = sda;
    i2c_cfg.sda_pullup_en = true;
    i2c_cfg.master.clk_speed = 100000;

    auto ret = i2c_param_config(port, &i2c_cfg);
    ret = ret ?: i2c_driver_install(port, I2C_MODE_MASTER, 0, 0, 0);
    if (ret != ESP_OK) {
        ESP_LOGI(TAG, "I2C init failed: 0x%x", ret);
        return ret;
    }

    bme_dev.intf_ptr = this;
    bme_dev.intf = BME68X_I2C_INTF;
    bme_dev.read = i2c_read;
    bme_dev.write = i2c_write;
    bme_dev.delay_us = sensor_delay_us;
    bme_dev.amb_temp = 25; // Do we need this?

    auto bme_ret = bme68x_init(&bme_dev);
    if (bme_ret < 0) {
        ESP_LOGE(TAG, "BME Init failed: %d", bme_ret);
        return ESP_FAIL;
    }

    return ret;
}

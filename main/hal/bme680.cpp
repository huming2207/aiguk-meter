#include <cstring>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_rom_sys.h>
#include <esp_log.h>

#include <driver/i2c.h>

#include "bme680.hpp"
#include "bme68x/bme68x.h"

#define I2C_WRITE_BIT 0
#define I2C_READ_BIT 1

BME68X_INTF_RET_TYPE bme680::i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    if (intf_ptr == nullptr) {
        return BME68X_E_NULL_PTR;
    }

    auto *ctx = static_cast<bme680 *>(intf_ptr);

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    auto ret = i2c_master_start(cmd);
    ret = ret ?: i2c_master_write_byte(cmd, (ctx->bme_addr << 1) | I2C_WRITE_BIT, true);
    ret = ret ?: i2c_master_write_byte(cmd, reg_addr, true);

    if (reg_data != nullptr && length > 0) {
        ret = ret ?: i2c_master_start(cmd);
        ret = ret ?: i2c_master_write_byte(cmd, (ctx->bme_addr << 1) | I2C_READ_BIT, true);
        ret = ret ?: i2c_master_read(cmd, reg_data, length, I2C_MASTER_LAST_NACK);
    }

    ret = ret ?: i2c_master_stop(cmd);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Read comm setup failed");
        return BME68X_E_COM_FAIL;
    }

    ret = ret ?: i2c_master_cmd_begin(ctx->port, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Read failed");
        return BME68X_E_COM_FAIL;
    } else {
        return BME68X_OK;
    }
}

BME68X_INTF_RET_TYPE bme680::i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    if (intf_ptr == nullptr) {
        return BME68X_E_NULL_PTR;
    }

    auto *ctx = static_cast<bme680 *>(intf_ptr);

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    auto ret = i2c_master_start(cmd);
    ret = ret ?: i2c_master_write_byte(cmd, (ctx->bme_addr << 1) | I2C_WRITE_BIT, true);
    ret = ret ?: i2c_master_write_byte(cmd, reg_addr, true);

    if (reg_data != nullptr && length > 0) {
        ret = ret ?: i2c_master_write(cmd, reg_data, length, true);
    }

    ret = ret ?: i2c_master_stop(cmd);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Write comm setup failed");
        return BME68X_E_COM_FAIL;
    }

    ret = ret ?: i2c_master_cmd_begin(ctx->port, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Write comm failed");
        return BME68X_E_COM_FAIL;
    } else {
        return BME68X_OK;
    }
}

void bme680::sensor_delay_us(uint32_t period, void *intf_ptr)
{
    (void)intf_ptr;

    if (period > 1000) {
        vTaskDelay(pdMS_TO_TICKS(period / 1000));
    } else {
        esp_rom_delay_us(period);
    }
}

esp_err_t bme680::init(gpio_num_t scl, gpio_num_t sda, uint8_t _addr, i2c_port_t _port)
{
    port = _port;
    bme_addr = _addr;

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
        return bme_ret;
    }

    return ret;
}

esp_err_t bme680::set_config(bme680_def::mode mode, bme68x_conf *_config, bme68x_heatr_conf *_heater_config)
{
    if (_config == nullptr || _heater_config == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    auto ret = bme68x_set_conf(_config, &bme_dev);
    if (ret < 0) {
        ESP_LOGE(TAG, "BME set config failed: %d", ret);
        return ret;
    }

    ret = bme68x_set_heatr_conf(mode, _heater_config, &bme_dev);
    if (ret < 0) {
        ESP_LOGE(TAG, "BME set heater config failed: %d", ret);
        return ret;
    }

    memcpy(&bme_config, _config, sizeof(bme_config));
    memcpy(&heater_config, _heater_config, sizeof(heater_config));
    return ESP_OK;
}

esp_err_t bme680::set_operation_mode(bme680_def::mode mode)
{
    auto ret = bme68x_set_op_mode(mode, &bme_dev);
    if (ret < 0) {
        ESP_LOGE(TAG, "BME set op mode failed: %d", ret);
        return ret;
    }

    return ESP_OK;
}

esp_err_t bme680::get_reading_forced(bme68x_data *out)
{
    if (out == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    auto ret = bme68x_set_op_mode(BME68X_FORCED_MODE, &bme_dev);
    if (ret < 0) {
        ESP_LOGE(TAG, "BME set op mode failed: %d", ret);
        return ret;
    }

    uint32_t delay_us = bme68x_get_meas_dur(BME68X_FORCED_MODE, &bme_config, &bme_dev);
    delay_us += (heater_config.heatr_dur * 1000);

    if (delay_us % 1000 != 0) {
        delay_us += 1000;
    }

    uint32_t delay_ticks = pdMS_TO_TICKS(delay_us / 1000) + 1;
    vTaskDelay(delay_ticks);

    uint8_t reading_cnt = 0;
    ret = bme68x_get_data(BME68X_FORCED_MODE, out, &reading_cnt, &bme_dev);
    if (ret < 0) {
        ESP_LOGE(TAG, "BME get data failed: %d", ret);
        return ret;
    }

    if (reading_cnt < 1) {
        ESP_LOGW(TAG, "No reading??");
    }

    return ESP_OK;
}

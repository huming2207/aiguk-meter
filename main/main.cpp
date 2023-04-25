#include <esp_log.h>
#include "mhz19e.hpp"
#include "bme680.hpp"

#define LOG_TAG "aiguk_main"

extern "C" void app_main(void)
{
    ESP_LOGI(LOG_TAG, "Started");

//    auto *co2_sensor = mhz19e::instance();
//    ESP_ERROR_CHECK(co2_sensor->init(GPIO_NUM_1, GPIO_NUM_2, UART_NUM_1));
//
//    while (true) {
//        uint16_t reading = 0;
//
//        ESP_ERROR_CHECK(co2_sensor->get_reading(&reading, pdMS_TO_TICKS(3000)));
//        ESP_LOGI(LOG_TAG, "Reading = %u ppm", reading);
//        vTaskDelay(pdMS_TO_TICKS(3000));
//    }

    bme680 *sensor = bme680::instance();
    ESP_ERROR_CHECK(sensor->init(GPIO_NUM_2, GPIO_NUM_1));

    bme68x_conf config = {};
    config.filter = BME68X_FILTER_OFF;
    config.odr = BME68X_ODR_NONE;
    config.os_hum = BME68X_OS_16X;
    config.os_pres = BME68X_OS_8X;
    config.os_temp = BME68X_OS_2X;

    bme68x_heatr_conf heater_config = {};
    heater_config.enable = BME68X_ENABLE;
    heater_config.heatr_temp = 300;
    heater_config.heatr_dur = 100;

    ESP_ERROR_CHECK(sensor->set_config(bme680_def::FORCED, &config, &heater_config));

    while (true) {
        bme68x_data data = {};
        ESP_ERROR_CHECK(sensor->get_reading_forced(&data));

        ESP_LOGI(LOG_TAG, "Got reading: %.2f degC; %.2f pa, %.2f %%; AQI %.2f", data.temperature, data.pressure, data.humidity, data.gas_resistance);

        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

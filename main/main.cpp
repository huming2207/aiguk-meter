#include <esp_log.h>
#include "mhz19e.hpp"
#include "bme680.hpp"
#include "display/weather_ui.hpp"

#define LOG_TAG "aiguk_main"

extern "C" void app_main(void)
{
    ESP_LOGI(LOG_TAG, "Started");

    auto *ui = weather_ui::instance();
    ESP_ERROR_CHECK(ui->init());
    ESP_ERROR_CHECK(ui->display_splash());
    vTaskDelay(pdMS_TO_TICKS(5000));

    while (true) {
        ESP_ERROR_CHECK(ui->display_main());
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

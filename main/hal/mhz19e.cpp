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

esp_err_t mhz19e::init(gpio_num_t tx, gpio_num_t rx)
{
    return ESP_OK;
}

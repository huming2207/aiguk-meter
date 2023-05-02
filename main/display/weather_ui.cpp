#include <esp_log.h>
#include "weather_ui.hpp"
#include "lvgl_wrapper.h"

esp_err_t weather_ui::init()
{
    auto ret = lvgl_disp_init();
    ret = ret ?: bme_sensor->init(GPIO_NUM_11, GPIO_NUM_12);
    bme68x_conf config = {};
    config.filter = BME68X_FILTER_OFF;
    config.odr = BME68X_ODR_NONE;
    config.os_hum = BME68X_OS_16X;
    config.os_pres = BME68X_OS_8X;
    config.os_temp = BME68X_OS_8X;

    bme68x_heatr_conf heater_config = {};
    heater_config.enable = BME68X_DISABLE;
    heater_config.heatr_temp = 300;
    heater_config.heatr_dur = 100;

    ret = ret ?: bme_sensor->set_config(bme680_def::FORCED, &config, &heater_config);

    if (ret == ESP_OK) {
        curr_state = ui_def::STATE_INIT;
        return display_main();
    }

    return ret;
}

esp_err_t weather_ui::display_splash()
{
    esp_err_t ret = ESP_OK;
    if (curr_state == ui_def::STATE_SPLASH) {
        return ret;
    }

    ret = clear_display();
    ret = ret ?: lvgl_take_lock(pdMS_TO_TICKS(1000));
    ret = ret ?: draw_two_bars(&top_bar, &bottom_bar, lv_color_hex(0x27632a), lv_color_hex(0xff9800));
    if (ret != ESP_OK) {
        lvgl_give_lock();
        return ret;
    }

    auto *top_text = lv_label_create(top_bar);
    lv_obj_align(top_text, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_align(top_text, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(top_text, "WeatherSense");
    lv_obj_set_style_text_font(top_text, &lv_font_montserrat_26, 0);
    lv_obj_set_style_text_color(top_text, lv_color_white(), 0);

    auto *bottom_text = lv_label_create(bottom_bar);
    lv_obj_align(bottom_text, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_align(bottom_text, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_fmt(bottom_text, "By Jackson Ming Hu\nCopyright (C) 2023\nFor My Beloved\n(Happy Birthday!)");
    lv_obj_set_style_text_font(bottom_text, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(bottom_text, lv_color_black(), 0);

    lvgl_give_lock();
    curr_state = ui_def::STATE_SPLASH;
    return ESP_OK;
}

esp_err_t weather_ui::display_main()
{
    ESP_LOGI(TAG, "Load main view");
    esp_err_t ret = read_sensor();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Sensor error: %d", ret);
    } else {
        ESP_LOGI(TAG, "Got reading: %.2f degC; %.2f pa, %.2f %%; AQI %.2f", bme_data.temperature, bme_data.pressure, bme_data.humidity, bme_data.gas_resistance);
    }

    if (curr_state != ui_def::STATE_MAIN) {
        ESP_LOGI(TAG, "Create main view");
        ret = clear_display();
        ret = ret ?: lvgl_take_lock(pdMS_TO_TICKS(1000));
        ret = ret ?: draw_three_bars(&top_bar, &mid_bar, &bottom_bar,
                                     lv_color_hex(0x27632a), lv_color_hex(0xff9800), lv_color_hex(0x005f56));
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Create main view failed");
            return ret;
        }

        ESP_LOGI(TAG, "Create widgets");
        top_title = lv_label_create(top_bar);
        mid_title = lv_label_create(mid_bar);
        bottom_title = lv_label_create(bottom_bar);

        if (top_title == nullptr || mid_title == nullptr || bottom_title == nullptr) {
            return ESP_ERR_NO_MEM;
        }

        lv_obj_align(top_title, LV_ALIGN_TOP_LEFT, 10, 5);
        lv_obj_set_style_text_align(top_title, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_font(top_title, &lv_font_montserrat_18, 0);
        lv_obj_set_style_text_color(top_title, lv_color_white(), 0);

        lv_obj_align(mid_title, LV_ALIGN_TOP_LEFT, 10, 5);
        lv_obj_set_style_text_align(mid_title, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_font(mid_title, &lv_font_montserrat_18, 0);
        lv_obj_set_style_text_color(mid_title, lv_color_white(), 0);

        lv_obj_align(bottom_title, LV_ALIGN_TOP_LEFT, 10, 5);
        lv_obj_set_style_text_align(bottom_title, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_font(bottom_title, &lv_font_montserrat_18, 0);
        lv_obj_set_style_text_color(bottom_title, lv_color_white(), 0);

        top_content = lv_label_create(top_bar);
        mid_content = lv_label_create(mid_bar);
        bottom_content = lv_label_create(bottom_bar);

        if (top_content == nullptr || mid_content == nullptr || bottom_content == nullptr) {
            return ESP_ERR_NO_MEM;
        }

        lv_obj_align(top_content, LV_ALIGN_TOP_LEFT, 10, 30);
        lv_obj_set_style_text_align(top_content, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_font(top_content, &lv_font_montserrat_42, 0);
        lv_obj_set_style_text_color(top_content, lv_color_white(), 0);

        lv_obj_align(mid_content, LV_ALIGN_TOP_LEFT, 10, 30);
        lv_obj_set_style_text_align(mid_content, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_font(mid_content, &lv_font_montserrat_42, 0);
        lv_obj_set_style_text_color(mid_content, lv_color_white(), 0);

        lv_obj_align(bottom_content, LV_ALIGN_TOP_LEFT, 10, 30);
        lv_obj_set_style_text_align(bottom_content, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_font(bottom_content, &lv_font_montserrat_42, 0);
        lv_obj_set_style_text_color(bottom_content, lv_color_white(), 0);

        curr_state = ui_def::STATE_MAIN;
    } else {
        ret = lvgl_take_lock(pdMS_TO_TICKS(1000));
    }

    if (ret != ESP_OK) {
        return ret;
    }

    lv_label_set_text(top_title, "Temperature (degC)");
    lv_label_set_text(mid_title, "Humidity (RH%)");
    lv_label_set_text(bottom_title, "Air Pressure (pa)");

    char content[16] = { 0 };
    snprintf(content, sizeof(content) - 1, "%.2f", bme_data.temperature - 5.6f);
    lv_label_set_text(top_content, content);
    memset(content, 0, sizeof(content));
    snprintf(content, sizeof(content) - 1, "%.2f", bme_data.humidity);
    lv_label_set_text(mid_content, content);
    memset(content, 0, sizeof(content));
    snprintf(content, sizeof(content) - 1, "%.2f", bme_data.pressure);
    lv_label_set_text(bottom_content, content);

    lvgl_give_lock();
    return ESP_OK;
}

esp_err_t weather_ui::clear_display()
{
    auto ret = lvgl_take_lock(pdMS_TO_TICKS(1000));
    if (ret != ESP_OK) {
        return ret;
    }

    if (root_obj != nullptr) {
        lv_obj_del(root_obj);
        top_bar = nullptr;
        mid_bar = nullptr;
        bottom_bar = nullptr;
    }

    root_obj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(root_obj, 240, 240);
    lv_obj_set_align(root_obj, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos(root_obj, 0, 0);
    lv_obj_set_style_radius(root_obj, 0, 0);
    lv_obj_set_scrollbar_mode(root_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_all(root_obj, 0, 0);
    lv_obj_set_style_border_width(root_obj, 0, 0);

    lvgl_give_lock();
    return ESP_OK;
}

esp_err_t weather_ui::read_sensor()
{
    return bme_sensor->get_reading_forced(&bme_data);
}

esp_err_t weather_ui::draw_two_bars(lv_obj_t **top_out, lv_obj_t **bottom_out, lv_color_t top_color, lv_color_t bottom_color)
{
    if (top_out == nullptr || bottom_out == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    if (root_obj == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }

    auto *top_obj = lv_obj_create(root_obj);
    if (top_obj == nullptr) {
        return ESP_FAIL;
    }

    lv_obj_set_size(top_obj, 240, 120);
    lv_obj_set_align(top_obj, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos(top_obj, 0, 0);
    lv_obj_set_style_radius(top_obj, 0, 0);
    lv_obj_set_style_bg_color(top_obj, top_color, 0);
    lv_obj_set_scrollbar_mode(top_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_all(top_obj, 0, 0);
    lv_obj_set_style_border_width(top_obj, 0, 0);
    *top_out = top_obj;

    auto *bottom_obj = lv_obj_create(root_obj);
    if (bottom_obj == nullptr) {
        return ESP_FAIL;
    }

    lv_obj_set_size(bottom_obj, 240, 120);
    lv_obj_set_align(bottom_obj, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos(bottom_obj, 0, 120);
    lv_obj_set_style_radius(bottom_obj, 0, 0);
    lv_obj_set_style_bg_color(bottom_obj, bottom_color, 0); // Dark cyan (need white text)
    lv_obj_set_scrollbar_mode(bottom_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_all(bottom_obj, 0, 0);
    lv_obj_set_style_border_width(bottom_obj, 0, 0);
    *bottom_out = bottom_obj;

    return ESP_OK;
}

esp_err_t weather_ui::draw_three_bars(lv_obj_t **top_out, lv_obj_t **mid_out, lv_obj_t **bottom_out, lv_color_t top_color, lv_color_t mid_color, lv_color_t bottom_color)
{
    if (top_out == nullptr || mid_out == nullptr || bottom_out == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    if (root_obj == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }

    auto *top_obj = lv_obj_create(root_obj);
    if (top_obj == nullptr) {
        return ESP_FAIL;
    }

    lv_obj_set_size(top_obj, 240, 80);
    lv_obj_set_align(top_obj, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos(top_obj, 0, 0);
    lv_obj_set_style_radius(top_obj, 0, 0);
    lv_obj_set_style_bg_color(top_obj, top_color, 0);
    lv_obj_set_scrollbar_mode(top_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_all(top_obj, 0, 0);
    lv_obj_set_style_border_width(top_obj, 0, 0);
    *top_out = top_obj;

    auto *mid_obj = lv_obj_create(root_obj);
    if (mid_obj == nullptr) {
        return ESP_FAIL;
    }

    lv_obj_set_size(mid_obj, 240, 80);
    lv_obj_set_align(mid_obj, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos(mid_obj, 0, 80);
    lv_obj_set_style_radius(mid_obj, 0, 0);
    lv_obj_set_style_bg_color(mid_obj, mid_color, 0); // Dark cyan (need white text)
    lv_obj_set_scrollbar_mode(mid_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_all(mid_obj, 0, 0);
    lv_obj_set_style_border_width(mid_obj, 0, 0);
    *mid_out = mid_obj;

    auto *bottom_obj = lv_obj_create(root_obj);
    if (bottom_obj == nullptr) {
        return ESP_FAIL;
    }

    lv_obj_set_size(bottom_obj, 240, 80);
    lv_obj_set_align(bottom_obj, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos(bottom_obj, 0, 160);
    lv_obj_set_style_radius(bottom_obj, 0, 0);
    lv_obj_set_style_bg_color(bottom_obj, bottom_color, 0); // Dark cyan (need white text)
    lv_obj_set_scrollbar_mode(bottom_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_all(bottom_obj, 0, 0);
    lv_obj_set_style_border_width(bottom_obj, 0, 0);
    *bottom_out = bottom_obj;

    return ESP_OK;
}

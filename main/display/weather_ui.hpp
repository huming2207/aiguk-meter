#pragma once

#include <cstdint>

#include <esp_err.h>
#include <lvgl.h>

namespace ui_def
{
    enum state : uint32_t
    {
        STATE_INIT = 0,
        STATE_SPLASH = 1,
        STATE_MAIN = 2,
    };
}

class weather_ui
{
public:
    static weather_ui *instance()
    {
        static weather_ui _instance;
        return &_instance;
    }

    weather_ui(weather_ui const &) = delete;
    void operator=(weather_ui const &) = delete;

private:
    weather_ui() = default;

public:
    esp_err_t init();

private:
    esp_err_t display_splash();
    esp_err_t display_main();
    esp_err_t clear_display();
    esp_err_t read_sensor();
    esp_err_t process_state();

private:
    esp_err_t draw_two_bars(lv_obj_t **top_out, lv_obj_t **bottom_out, lv_color_t top_color, lv_color_t bottom_color);
    esp_err_t draw_three_bars(lv_obj_t **top_out, lv_obj_t **mid_out, lv_obj_t **bottom_out, lv_color_t top_color, lv_color_t mid_color, lv_color_t bottom_color);

private:
    static const constexpr char TAG[] = "weather_ui";
    ui_def::state curr_state = ui_def::STATE_SPLASH;
    lv_obj_t *root_obj = nullptr;
    lv_obj_t *top_bar = nullptr;
    lv_obj_t *mid_bar = nullptr;
    lv_obj_t *bottom_bar = nullptr;
    lv_obj_t *top_title = nullptr;
    lv_obj_t *mid_title = nullptr;
    lv_obj_t *bottom_title = nullptr;
    lv_obj_t *top_content = nullptr;
    lv_obj_t *mid_content = nullptr;
    lv_obj_t *bottom_content = nullptr;
};

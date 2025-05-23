//
// Created by Amitosh Swain Mahapatra on 07/03/25.
//

#ifndef LIGHTINGAPP_HPP
#define LIGHTINGAPP_HPP
#include <lvgl.h>

#include "App.hpp"
#include "WizBulb.hpp"

class LightingApp : public App {
public:
    explicit LightingApp(lvgl_m5_dial_t* d);
    void start(lv_obj_t* screen) override;
    static void screenSwipeCallback(lv_event_t* e);
    void stop() override;
    void update() override;

private:
    lv_obj_t* color_screen = nullptr;
    lv_obj_t* arcCanvas    = nullptr;
    lv_obj_t* arcSlider    = nullptr;
    lv_obj_t* toggleBtn    = nullptr;
    lv_obj_t* bulb_icon    = nullptr;
    lv_obj_t* cct_label    = nullptr;
    lv_group_t* group      = nullptr;

    // Brightness control
    lv_obj_t* brightness_screen = nullptr;
    lv_obj_t* brightnessSlider  = nullptr;
    lv_obj_t* brightnessLabel   = nullptr;
    lv_group_t* brightnessGroup = nullptr;

    int current_color    = 0;
    int current_cct      = 31;
    bool is_rgb_mode     = true;
    WizBulb wizBulb;
    lv_timer_t* stateRequestTimer = nullptr;
    bool bulbOnline = false;

    void update_display() const;

    static void toggle_mode(lv_event_t* e);
    static void arc_event_cb(lv_event_t* e);
    static void onBrightnessChangedCallback(lv_event_t* e);

    static void setBrightness(void* data);
    static void changeColor(void* data);
    
    // Handle bulb state updates
    void onBulbStateUpdate(const WizBulbState& state);
    static void requestBulbState(lv_timer_t* timer);
};

#endif  // LIGHTINGAPP_HPP

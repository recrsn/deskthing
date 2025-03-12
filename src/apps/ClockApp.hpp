//
// Created by Amitosh Swain Mahapatra on 11/02/25.
//

#ifndef CLOCKAPP_H
#define CLOCKAPP_H

#include <lvgl.h>

#include "App.hpp"
#include "WizBulb.hpp"

class ClockApp final : public App {
public:
    explicit ClockApp(lvgl_m5_dial_t *dial);
    static void setBrightnessCallback(void *data);
    void start(lv_obj_t *parent) override;
    void stop() override;

private:
    static void updateTime(lv_timer_t *timer);
    void update();
    static void onBrightnessChangedCallback(lv_event_t *e);
    lv_obj_t *timeLabel         = nullptr;
    lv_obj_t *dateLabel         = nullptr;
    lv_obj_t *wifiIcon          = nullptr;
    lv_obj_t *brightnessSlider  = nullptr;
    lv_obj_t *brightnessLabel   = nullptr;
    lv_group_t *group           = nullptr;
    lv_timer_t *overlayTimer    = nullptr;
    lv_obj_t *brightnessOverlay = nullptr;
    lv_timer_t *clockTimer      = nullptr;
    WizBulb wizBulb;
    lv_timer_t* stateRequestTimer = nullptr;
    bool bulbOnline = false;
    
    // Handle bulb state updates
    void onBulbStateUpdate(const WizBulbState& state);
    static void requestBulbState(lv_timer_t* timer);
};

#endif  // CLOCKAPP_H

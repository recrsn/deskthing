//
// Created by Amitosh Swain Mahapatra on 11/02/25.
//

#ifndef CLOCKAPP_H
#define CLOCKAPP_H

#include <lvgl.h>

#include "App.hpp"

class ClockApp final : public App {
public:
    explicit ClockApp(lvgl_m5_dial_t *dial) : App(dial) {}
    static void setBrightnessCallback(void *data);
    void start(lv_obj_t *parent) override;
    void stop() override;

private:
    static void update(lv_timer_t *timer);
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
};

#endif  // CLOCKAPP_H

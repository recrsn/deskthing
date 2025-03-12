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
    void update() override;

private:
    static void updateTime(lv_timer_t *timer);
    static void onBrightnessChangedCallback(lv_event_t *e);
    static void powerButtonCallback(lv_event_t *e) {
        auto *self = static_cast<ClockApp *>(lv_event_get_user_data(e));
        lv_async_call(setPowerCallback, self);
    }

    static void setPowerCallback(void *data) {
        auto *self = static_cast<ClockApp *>(data);

        // Send power on command to the bulb
        char payload[100];
        snprintf(payload, sizeof(payload), R"({"method":"setPilot","params":{"state":true}})");

        // Use the sendCommand method through a wrapper since it's private
        self->wizBulb.requestState();  // This will trigger a state update

        // Hide power button immediately for better UX
        lv_obj_add_flag(self->powerButton, LV_OBJ_FLAG_HIDDEN);
    }

    lv_obj_t *timeLabel           = nullptr;
    lv_obj_t *dateLabel           = nullptr;
    lv_obj_t *wifiIcon            = nullptr;
    lv_obj_t *overlay             = nullptr;
    lv_obj_t *offlineOverlay      = nullptr;
    lv_obj_t *powerButton         = nullptr;
    lv_obj_t *offlineMsg          = nullptr;
    lv_obj_t *offlineIcon         = nullptr;
    lv_obj_t *brightnessOverlay   = nullptr;
    lv_obj_t *brightnessSlider    = nullptr;
    lv_obj_t *brightnessLabel     = nullptr;
    lv_obj_t *powerOffOverlay     = nullptr;
    lv_obj_t* bulbIcon = nullptr;
    lv_group_t *group             = nullptr;
    lv_timer_t *overlayTimer      = nullptr;
    lv_timer_t *clockTimer        = nullptr;
    lv_timer_t *stateRequestTimer = nullptr;

    WizBulb wizBulb;
    lv_style_t overlayStyle;

    bool bulbOnline  = false;
    bool bulbPowered = false;

    // Handle bulb state updates
    void onBulbStateUpdate(const WizBulbState &state);
    static void requestBulbState(lv_timer_t *timer);
    void updateBulbStatusUI() const;
};

#endif  // CLOCKAPP_H

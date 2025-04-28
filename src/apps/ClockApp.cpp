//
// Created by Amitosh Swain Mahapatra on 11/02/25.
//
#include "ClockApp.hpp"

#include <lvgl.h>

#ifdef ESP_PLATFORM
#include <lwip/err.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <resources/fontawesome.h>

#include "lvgl_port_m5stack.hpp"
#include "platform/log.h"

static const char *LOG_TAG = "ClockApp";

ClockApp::ClockApp(lvgl_m5_dial_t *dial) : App(dial) {
    LOG_I(LOG_TAG, "Initializing ClockApp");
    // Set up the callback
    wizBulb.setStateCallback([this](const WizBulbState &state) { this->onBulbStateUpdate(state); });
}

void ClockApp::setBrightnessCallback(void *data) {
    const auto arc        = static_cast<lv_obj_t *>(data);
    const auto app        = static_cast<ClockApp *>(lv_obj_get_user_data(arc));
    const auto brightness = lv_arc_get_value(arc);

    LOG_D(LOG_TAG, "Brightness callback with value: %d", brightness);
    app->wizBulb.setBrightness(brightness);
}

void ClockApp::start(lv_obj_t *screen) {
    LOG_I(LOG_TAG, "Starting ClockApp");

    lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), 0);
    lv_style_init(&this->overlayStyle);
    lv_style_set_bg_color(&this->overlayStyle, lv_color_hex(0x000000));
    lv_style_set_border_width(&this->overlayStyle, 0);
    lv_style_set_radius(&this->overlayStyle, 0);
    lv_style_set_pad_all(&this->overlayStyle, 0);

    // Time label
    timeLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(timeLabel, &lv_font_montserrat_40, 0);
    lv_obj_align(timeLabel, LV_ALIGN_TOP_MID, 0, 40);

    // Date label
    dateLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(dateLabel, &lv_font_montserrat_18, 0);
    lv_obj_align(dateLabel, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *iconContainer = lv_obj_create(screen);
    lv_obj_set_size(iconContainer, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(iconContainer, LV_ALIGN_BOTTOM_MID, 0, -40);
    lv_obj_set_layout(iconContainer, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(iconContainer, LV_FLEX_FLOW_ROW);
    lv_obj_add_style(iconContainer, &this->overlayStyle, 0);

    // WiFi icon
    wifiIcon = lv_label_create(iconContainer);
    lv_label_set_text(wifiIcon, LV_SYMBOL_WIFI);
    lv_obj_add_flag(wifiIcon, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_set_style_text_color(wifiIcon, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_margin_right(wifiIcon, 10, 0);

    bulbIcon = lv_label_create(iconContainer);
    lv_label_set_text(bulbIcon, LV_SYMBOL_LIGHTBULB);
    lv_obj_set_style_text_font(bulbIcon, &fontawesome, 0);
    lv_obj_add_flag(bulbIcon, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    // overlay
    overlay = lv_obj_create(screen);
    lv_obj_remove_flag(overlay, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(overlay, LV_OBJ_FLAG_FLOATING);
    lv_obj_add_flag(overlay, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_size(overlay, LV_PCT(100), LV_PCT(100));
    lv_obj_center(overlay);
    lv_obj_add_style(overlay, &this->overlayStyle, 0);

    powerOffOverlay = lv_obj_create(overlay);
    lv_obj_remove_flag(powerOffOverlay, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(powerOffOverlay, LV_OBJ_FLAG_FLOATING);
    lv_obj_add_flag(powerOffOverlay, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_size(powerOffOverlay, LV_PCT(100), LV_PCT(100));
    lv_obj_center(powerOffOverlay);
    lv_obj_add_style(powerOffOverlay, &this->overlayStyle, 0);

    // Power button
    powerButton = lv_btn_create(powerOffOverlay);
    lv_obj_set_size(powerButton, 120, 120);
    lv_obj_set_style_radius(powerButton, 60, 0);
    lv_obj_set_style_bg_color(powerButton, lv_color_hex(0x404040), 0);
    lv_obj_set_style_bg_color(powerButton, lv_color_hex(0x606060), LV_STATE_PRESSED);
    lv_obj_add_event_cb(powerButton, powerButtonCallback, LV_EVENT_CLICKED, this);
    lv_obj_center(powerButton);

    lv_obj_t *powerIcon = lv_label_create(powerButton);
    lv_label_set_text(powerIcon, LV_SYMBOL_POWER);
    lv_obj_set_style_text_font(powerIcon, &fontawesome_64, 0);
    lv_obj_center(powerIcon);

    offlineOverlay = lv_obj_create(overlay);
    lv_obj_remove_flag(offlineOverlay, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(offlineOverlay, LV_OBJ_FLAG_FLOATING);
    lv_obj_add_flag(offlineOverlay, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_size(offlineOverlay, LV_PCT(100), LV_PCT(100));
    lv_obj_center(offlineOverlay);
    lv_obj_add_style(offlineOverlay, &this->overlayStyle, 0);

    // Offline message
    offlineMsg = lv_label_create(offlineOverlay);
    lv_label_set_text(offlineMsg, "Bulb is offline");
    lv_obj_set_style_text_font(offlineMsg, &lv_font_montserrat_16, 0);
    lv_obj_align(offlineMsg, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_flag(offlineMsg, LV_OBJ_FLAG_HIDDEN);  // Initially hidden

    // Offline icon
    offlineIcon = lv_label_create(offlineOverlay);
    lv_label_set_text(offlineIcon, LV_SYMBOL_WARNING);
    lv_obj_set_style_text_font(offlineIcon, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(offlineIcon, lv_color_hex(0xFF5500), 0);
    lv_obj_align_to(offlineIcon, offlineMsg, LV_ALIGN_OUT_TOP_MID, 0, -10);
    lv_obj_add_flag(offlineIcon, LV_OBJ_FLAG_HIDDEN);  // Initially hidden

    // brightness overlay
    brightnessOverlay = lv_obj_create(overlay);
    lv_obj_remove_flag(brightnessOverlay, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(brightnessOverlay, LV_OBJ_FLAG_FLOATING);
    lv_obj_add_flag(brightnessOverlay, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_size(brightnessOverlay, LV_PCT(100), LV_PCT(100));
    lv_obj_center(brightnessOverlay);
    lv_obj_add_style(brightnessOverlay, &this->overlayStyle, 0);

    brightnessSlider = lv_arc_create(brightnessOverlay);
    lv_obj_set_size(brightnessSlider, 200, 200);
    lv_obj_center(brightnessSlider);
    lv_arc_set_value(brightnessSlider, 50);
    lv_obj_set_state(brightnessSlider, LV_STATE_FOCUSED, true);

    brightnessLabel = lv_label_create(brightnessOverlay);
    lv_obj_set_style_text_font(brightnessLabel, &lv_font_montserrat_40, 0);
    lv_label_set_text(brightnessLabel, "50%");
    lv_obj_center(brightnessLabel);

    lv_obj_t *title = lv_label_create(brightnessOverlay);
    lv_label_set_text(title, "Brightness");
    lv_obj_align_to(title, brightnessLabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    group = lv_group_create();
    lv_group_add_obj(group, brightnessSlider);
    lv_group_focus_obj(brightnessSlider);
    lv_group_set_editing(group, true);
    lv_group_focus_freeze(group, true);
    lv_indev_set_group(dial->encoder, group);

    lv_obj_set_user_data(brightnessSlider, this);
    lv_obj_add_event_cb(brightnessSlider, onBrightnessChangedCallback, LV_EVENT_VALUE_CHANGED, this);

    overlayTimer = lv_timer_create(
        [](lv_timer_t *timer) {
            const auto self = static_cast<ClockApp *>(lv_timer_get_user_data(timer));
            lv_obj_add_flag(self->overlay, LV_OBJ_FLAG_HIDDEN);
        },
        5000, this);
    lv_timer_set_auto_delete(overlayTimer, false);
    lv_timer_pause(overlayTimer);

    clockTimer = lv_timer_create(updateTime, 1000, this);
    lv_timer_ready(clockTimer);

    // Create timer to periodically request bulb state
    stateRequestTimer = lv_timer_create(requestBulbState, 10000, this);

    // Request initial state
    wizBulb.requestState();
}

void ClockApp::onBrightnessChangedCallback(lv_event_t *e) {
    const auto app      = static_cast<ClockApp *>(lv_event_get_user_data(e));
    const lv_obj_t *arc = lv_event_get_target_obj(e);
    lv_label_set_text_fmt(app->brightnessLabel, "%d%%", lv_arc_get_value(arc));
    lv_timer_reset(app->overlayTimer);
    lv_timer_resume(app->overlayTimer);

    // Send brightness command to Wiz light
    lv_async_call(setBrightnessCallback, (void *)arc);
}

void ClockApp::updateTime(lv_timer_t *timer) {
    const auto app   = static_cast<ClockApp *>(lv_timer_get_user_data(timer));
    const time_t now = time(nullptr);
    const tm *t      = localtime(&now);

    static char timeStr[6];
    strftime(timeStr, sizeof(timeStr), "%H:%M", t);
    lv_label_set_text(app->timeLabel, timeStr);

    static char dateStr[20];
    strftime(dateStr, sizeof(dateStr), "%a, %d %b", t);
    lv_label_set_text(app->dateLabel, dateStr);
}

void ClockApp::update() {
    if (lv_obj_has_flag(this->overlay, LV_OBJ_FLAG_HIDDEN)) {
        lv_indev_data_t data;
        const auto indev_get_read_cb = lv_indev_get_read_cb(this->dial->encoder);
        indev_get_read_cb(this->dial->encoder, &data);
        if (data.enc_diff) {
            lv_obj_clear_flag(this->overlay, LV_OBJ_FLAG_HIDDEN);
            if (bulbOnline && bulbPowered) {
                lv_arc_set_value(this->brightnessSlider, lv_arc_get_value(this->brightnessSlider) + data.enc_diff);
                lv_obj_send_event(this->brightnessSlider, LV_EVENT_VALUE_CHANGED, nullptr);
            }
            lv_timer_reset(overlayTimer);
            lv_timer_resume(overlayTimer);
        }
    }
    wizBulb.update();
}

void ClockApp::onBulbStateUpdate(const WizBulbState &state) {
    bulbOnline  = state.isOnline;
    bulbPowered = state.isOn;

    LOG_D(LOG_TAG, "Bulb state update - Online: %s, Powered: %s, Brightness: %d", bulbOnline ? "yes" : "no",
          bulbPowered ? "yes" : "no", state.brightness);

    // Update UI based on bulb state
    updateBulbStatusUI();

    if (bulbOnline) {
        // Bulb is online, update UI with current state
        lv_arc_set_value(brightnessSlider, state.brightness);
        lv_label_set_text_fmt(brightnessLabel, "%d%%", state.brightness);
    }
}

void ClockApp::updateBulbStatusUI() const {
    if (!bulbOnline) {
        LOG_D(LOG_TAG, "Updating UI for offline bulb");
        lv_obj_set_style_text_color(bulbIcon, lv_color_hex(0xcc0000), 0);
        lv_obj_clear_flag(offlineOverlay, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(powerOffOverlay, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(brightnessOverlay, LV_OBJ_FLAG_HIDDEN);
    } else if (!bulbPowered) {
        LOG_D(LOG_TAG, "Updating UI for powered-off bulb");
        lv_obj_set_style_text_color(bulbIcon, lv_color_hex(0x888888), 0);
        lv_obj_clear_flag(powerOffOverlay, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(offlineOverlay, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(brightnessOverlay, LV_OBJ_FLAG_HIDDEN);
    } else {
        LOG_D(LOG_TAG, "Updating UI for powered-on bulb");
        lv_obj_set_style_text_color(bulbIcon, lv_color_hex(0xCCCC00), 0);
        lv_obj_clear_flag(brightnessOverlay, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(powerOffOverlay, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(offlineOverlay, LV_OBJ_FLAG_HIDDEN);
    }
}

void ClockApp::requestBulbState(lv_timer_t *timer) {
    auto *self = static_cast<ClockApp *>(lv_timer_get_user_data(timer));
    self->wizBulb.requestState();
}

void ClockApp::stop() {
    LOG_I(LOG_TAG, "Stopping ClockApp");

    if (overlayTimer) {
        lv_timer_del(overlayTimer);
    }

    if (clockTimer) {
        lv_timer_del(clockTimer);
    }

    if (group) {
        lv_group_del(group);
    }

    if (stateRequestTimer) {
        lv_timer_del(stateRequestTimer);
        stateRequestTimer = nullptr;
    }

    // Clean up UI elements
    if (powerButton) {
        lv_obj_del(powerButton);
        powerButton = nullptr;
    }

    if (offlineMsg) {
        lv_obj_del(offlineMsg);
        offlineMsg = nullptr;
    }

    if (offlineIcon) {
        lv_obj_del(offlineIcon);
        offlineIcon = nullptr;
    }

    LOG_I(LOG_TAG, "ClockApp stopped");
}

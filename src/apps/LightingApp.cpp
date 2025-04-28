//
// Created by Amitosh Swain Mahapatra on 07/03/25.
//

#include "LightingApp.hpp"

#include "platform/log.h"
#include "resources/cct_arc.h"
#include "resources/fontawesome.h"
#include "resources/hsv_circle.h"

static const char* LOG_TAG = "LightingApp";

LightingApp::LightingApp(lvgl_m5_dial_t* d) : App(d) {
    LOG_I(LOG_TAG, "Initializing LightingApp");
    // Set up the callback
    wizBulb.setStateCallback([this](const WizBulbState& state) { this->onBulbStateUpdate(state); });
}

void LightingApp::start(lv_obj_t* screen) {
    LOG_I(LOG_TAG, "Starting LightingApp");

    color_screen = lv_obj_create(screen);
    lv_obj_set_size(color_screen, 240, 240);
    lv_obj_set_style_bg_color(color_screen, lv_color_hex(0x000000), 0);
    lv_obj_clear_flag(color_screen, LV_OBJ_FLAG_SCROLLABLE);

    arcCanvas = lv_image_create(color_screen);
    lv_image_set_src(arcCanvas, &cct_arc);
    lv_obj_center(arcCanvas);

    bulb_icon = lv_label_create(color_screen);
    lv_label_set_text(bulb_icon, LV_SYMBOL_LIGHTBULB);
    lv_obj_set_style_text_font(bulb_icon, &fontawesome, 0);
    lv_obj_align(bulb_icon, LV_ALIGN_CENTER, 0, 0);

    cct_label = lv_label_create(color_screen);
    lv_obj_align_to(cct_label, bulb_icon, LV_ALIGN_TOP_MID, 0, -20);
    lv_obj_add_flag(cct_label, LV_OBJ_FLAG_HIDDEN);

    arcSlider = lv_arc_create(color_screen);
    lv_obj_set_size(arcSlider, 200, 200);
    lv_obj_center(arcSlider);
    lv_obj_add_event_cb(arcSlider, arc_event_cb, LV_EVENT_VALUE_CHANGED, this);
    lv_obj_set_style_arc_opa(arcSlider, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_arc_opa(arcSlider, LV_OPA_TRANSP, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(arcSlider, lv_color_hex(0xFFFFFF), LV_PART_KNOB);
    lv_obj_add_flag(arcSlider, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_remove_flag(arcSlider, LV_OBJ_FLAG_GESTURE_BUBBLE);

    toggleBtn = lv_switch_create(color_screen);
    lv_obj_set_size(toggleBtn, 40, 20);
    lv_obj_align_to(toggleBtn, bulb_icon, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_add_event_cb(toggleBtn, toggle_mode, LV_EVENT_VALUE_CHANGED, this);
    lv_obj_set_state(toggleBtn, LV_STATE_CHECKED, true);

    lv_obj_t* rgbLabel = lv_label_create(color_screen);
    lv_label_set_text(rgbLabel, "RGB");
    lv_obj_set_style_text_font(rgbLabel, &lv_font_montserrat_12, 0);
    lv_obj_align_to(rgbLabel, toggleBtn, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    lv_obj_t* cctLabel = lv_label_create(color_screen);
    lv_label_set_text(cctLabel, "CCT");
    lv_obj_set_style_text_font(cctLabel, &lv_font_montserrat_12, 0);
    lv_obj_align_to(cctLabel, toggleBtn, LV_ALIGN_OUT_LEFT_MID, -5, 0);

    update_display();

    // Create brightness screen
    brightness_screen = lv_obj_create(screen);
    lv_obj_remove_flag(brightness_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(brightness_screen, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_size(brightness_screen, LV_PCT(100), LV_PCT(100));
    lv_obj_center(brightness_screen);
    lv_obj_set_style_bg_color(brightness_screen, lv_color_hex(0x000000), 0);

    brightnessSlider = lv_arc_create(brightness_screen);
    lv_obj_set_size(brightnessSlider, 200, 200);
    lv_obj_center(brightnessSlider);
    lv_arc_set_value(brightnessSlider, 50);
    lv_obj_set_state(brightnessSlider, LV_STATE_FOCUSED, true);
    lv_obj_add_flag(brightnessSlider, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_remove_flag(brightnessSlider, LV_OBJ_FLAG_GESTURE_BUBBLE);

    brightnessLabel = lv_label_create(brightness_screen);
    lv_obj_set_style_text_font(brightnessLabel, &lv_font_montserrat_40, 0);
    lv_label_set_text(brightnessLabel, "50%");
    lv_obj_center(brightnessLabel);

    lv_obj_t* title = lv_label_create(brightness_screen);
    lv_label_set_text(title, "Brightness");
    lv_obj_align_to(title, brightnessLabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_obj_add_event_cb(brightnessSlider, onBrightnessChangedCallback, LV_EVENT_VALUE_CHANGED, this);

    group = lv_group_create();
    lv_group_add_obj(group, arcSlider);
    lv_group_focus_obj(arcSlider);
    lv_group_set_editing(group, true);
    lv_group_focus_freeze(group, true);
    lv_indev_set_group(dial->encoder, group);

    brightnessGroup = lv_group_create();
    lv_group_add_obj(brightnessGroup, brightnessSlider);
    lv_group_set_editing(brightnessGroup, true);
    lv_group_focus_freeze(brightnessGroup, true);

    lv_obj_add_event_cb(screen, screenSwipeCallback, LV_EVENT_GESTURE, this);

    // Create timer to periodically request bulb state
    stateRequestTimer = lv_timer_create(requestBulbState, 10000, this);  // Request every 10 seconds

    // Request initial state
    wizBulb.requestState();
}

void LightingApp::screenSwipeCallback(lv_event_t* e) {
    const auto* self = static_cast<LightingApp*>(lv_event_get_user_data(e));
    const auto dir   = lv_indev_get_gesture_dir(lv_indev_active());

    switch (dir) {
        case LV_DIR_LEFT:
        case LV_DIR_BOTTOM:
            lv_obj_clear_flag(self->brightness_screen, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(self->color_screen, LV_OBJ_FLAG_HIDDEN);
            lv_indev_set_group(self->dial->encoder, self->brightnessGroup);
            lv_group_focus_obj(self->brightnessSlider);
            break;
        case LV_DIR_RIGHT:
        case LV_DIR_TOP:
            lv_obj_clear_flag(self->color_screen, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(self->brightness_screen, LV_OBJ_FLAG_HIDDEN);
            lv_indev_set_group(self->dial->encoder, self->group);
            lv_group_focus_obj(self->arcSlider);
            break;
        default:
            LV_LOG("Unknown gesture");
    }
}

void LightingApp::toggle_mode(lv_event_t* e) {
    auto* self        = static_cast<LightingApp*>(lv_event_get_user_data(e));
    self->is_rgb_mode = !self->is_rgb_mode;
    LOG_I(LOG_TAG, "Light mode toggled to %s", self->is_rgb_mode ? "RGB" : "CCT");
    lv_async_call(changeColor, self);
    self->update_display();
}

void LightingApp::arc_event_cb(lv_event_t* e) {
    auto* self    = static_cast<LightingApp*>(lv_event_get_user_data(e));
    int32_t value = lv_arc_get_value(self->arcSlider);

    if (self->is_rgb_mode) {
        self->current_color = value;
        LOG_D(LOG_TAG, "Color hue adjusted to: %d", value);
        if (value == 0) {
            lv_arc_set_value(self->arcSlider, 360);
        } else if (value == 360) {
            lv_arc_set_value(self->arcSlider, 0);
        }
    } else {
        self->current_cct = value;
        LOG_D(LOG_TAG, "Color temperature adjusted to: %dK", self->current_cct * 100);
        lv_label_set_text_fmt(self->cct_label, "%dK", self->current_cct * 100);
    }

    lv_async_call(changeColor, self);
    self->update_display();
}

void LightingApp::update_display() const {
    LOG_D(LOG_TAG, "Updating display for %s mode", this->is_rgb_mode ? "RGB" : "CCT");

    if (this->is_rgb_mode) {
        lv_image_set_src(this->arcCanvas, &hsv_circle);
        lv_obj_add_flag(this->cct_label, LV_OBJ_FLAG_HIDDEN);
        lv_arc_set_range(this->arcSlider, 0, 360);
        lv_arc_set_angles(this->arcSlider, 270, 269);
        lv_arc_set_bg_angles(this->arcSlider, 270, 269);
        lv_arc_set_value(this->arcSlider, this->current_color);
    } else {
        lv_image_set_src(this->arcCanvas, &cct_arc);
        lv_obj_clear_flag(this->cct_label, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text_fmt(this->cct_label, "%dK", this->current_cct * 100);
        lv_arc_set_range(this->arcSlider, 22, 65);
        lv_arc_set_angles(this->arcSlider, 135, 45);
        lv_arc_set_bg_angles(this->arcSlider, 135, 45);
        lv_arc_set_value(this->arcSlider, this->current_cct);
    }
}

void LightingApp::changeColor(void* data) {
    auto* self = static_cast<LightingApp*>(data);
    if (self->is_rgb_mode) {
        auto [blue, green, red] = lv_color_hsv_to_rgb(self->current_color, 100, 100);
        LOG_I(LOG_TAG, "Changing color to RGB(%d, %d, %d) from HSV(%d, 100, 100)", red, green, blue,
              self->current_color);
        self->wizBulb.setColor(red, green, blue);
    } else {
        LOG_I(LOG_TAG, "Changing color temperature to %dK", self->current_cct * 100);
        self->wizBulb.setColorTemperature(self->current_cct * 100);
    }
}

void LightingApp::setBrightness(void* data) {
    auto* self      = static_cast<LightingApp*>(data);
    auto brightness = lv_arc_get_value(self->brightnessSlider);

    LOG_I(LOG_TAG, "Setting brightness to %d%%", brightness);
    self->wizBulb.setBrightness(brightness);
}

void LightingApp::onBrightnessChangedCallback(lv_event_t* e) {
    const auto app      = static_cast<LightingApp*>(lv_event_get_user_data(e));
    const lv_obj_t* arc = lv_event_get_target_obj(e);
    lv_label_set_text_fmt(app->brightnessLabel, "%d%%", lv_arc_get_value(arc));

    // Send brightness command to Wiz light
    lv_async_call(setBrightness, app);
}

void LightingApp::update() {}

void LightingApp::onBulbStateUpdate(const WizBulbState& state) {
    bulbOnline = state.isOnline;

    LOG_D(LOG_TAG, "Bulb state update - Online: %s", bulbOnline ? "yes" : "no");

    // Update UI based on bulb state
    if (bulbOnline) {
        LOG_D(LOG_TAG, "Updating UI with online bulb state");
        // Bulb is online, update UI with current state
        lv_obj_set_style_text_color(bulb_icon, lv_color_hex(0xFFFFFF), 0);

        // Update brightness slider
        lv_arc_set_value(brightnessSlider, state.brightness);
        lv_label_set_text_fmt(brightnessLabel, "%d%%", state.brightness);

        // Update color/temperature based on mode
        is_rgb_mode = state.isRGBMode;
        lv_obj_set_state(toggleBtn, is_rgb_mode ? LV_STATE_CHECKED : LV_STATE_DEFAULT, true);

        if (is_rgb_mode) {
            // Convert RGB to HSV and update color slider
            auto [h, s, v] = lv_color_rgb_to_hsv(state.red, state.green, state.blue);
            current_color  = h;
            LOG_D(LOG_TAG, "Received RGB mode: R=%d, G=%d, B=%d (HSV: %d, %d, %d)", state.red, state.green, state.blue,
                  h, s, v);
            lv_arc_set_value(arcSlider, current_color);
        } else {
            // Update temperature slider
            current_cct = state.temperature / 100;
            LOG_D(LOG_TAG, "Received temperature mode: %d K", state.temperature);
            lv_arc_set_value(arcSlider, current_cct);
            lv_label_set_text_fmt(cct_label, "%dK", current_cct * 100);
        }

        update_display();
    } else {
        // Bulb is offline, update UI to show offline state
        LOG_W(LOG_TAG, "Bulb appears to be offline");
        lv_obj_set_style_text_color(bulb_icon, lv_color_hex(0x888888), 0);
    }
}

void LightingApp::requestBulbState(lv_timer_t* timer) {
    auto* self = static_cast<LightingApp*>(lv_timer_get_user_data(timer));
    self->wizBulb.requestState();
}

void LightingApp::stop() {
    LOG_I(LOG_TAG, "Stopping LightingApp");

    if (group) {
        lv_group_del(group);
    }

    if (brightnessGroup) {
        lv_group_del(brightnessGroup);
    }

    if (stateRequestTimer) {
        lv_timer_del(stateRequestTimer);
        stateRequestTimer = nullptr;
    }

    LOG_I(LOG_TAG, "LightingApp resources cleaned up");
}

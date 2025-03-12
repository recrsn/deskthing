#include "SettingsApp.hpp"
#include <lvgl.h>

#ifdef ESP_PLATFORM
#include <M5Unified.h> // For M5.Display, M5.Speaker, etc.
#endif

SettingsApp::SettingsApp(lvgl_m5_dial_t *d) : App(d) {}

void SettingsApp::start(lv_obj_t *parent) {
    // Keep overall size ~180x180
    lv_obj_set_size(parent, 180, 180);
    lv_obj_set_style_bg_color(parent, lv_color_hex(0x000000), 0);

    // Load saved settings first
    loadSettings();
    applySettings();

    // Create brightness slider
    brightnessSlider = lv_slider_create(parent);
    lv_obj_align(brightnessSlider, LV_ALIGN_TOP_MID, 0, 20);
    lv_slider_set_range(brightnessSlider, 0, 255);
    lv_slider_set_value(brightnessSlider, 128, LV_ANIM_OFF);
    lv_obj_add_event_cb(brightnessSlider, onBrightnessChanged, LV_EVENT_VALUE_CHANGED, this);

    // Create volume slider
    volumeSlider = lv_slider_create(parent);
    lv_obj_align_to(volumeSlider, brightnessSlider, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);
    lv_slider_set_range(volumeSlider, 0, 100);
    lv_slider_set_value(volumeSlider, 50, LV_ANIM_OFF);
    lv_obj_add_event_cb(volumeSlider, onVolumeChanged, LV_EVENT_VALUE_CHANGED, this);

    // Create timezone dropdown
    timezoneDropdown = lv_dropdown_create(parent);
    lv_dropdown_set_options(timezoneDropdown,
                            "UTC-12\nUTC-11\nUTC-10\nUTC-9\nUTC-8\n"
                            "UTC-7\nUTC-6\nUTC-5\nUTC-4\nUTC-3\n"
                            "UTC-2\nUTC-1\nUTC+0\nUTC+1\nUTC+2\n"
                            "UTC+3\nUTC+4\nUTC+5\nUTC+6\nUTC+7\n"
                            "UTC+8\nUTC+9\nUTC+10\nUTC+11\nUTC+12");
    lv_obj_align_to(timezoneDropdown, volumeSlider, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);
    lv_obj_add_event_cb(timezoneDropdown, onTimezoneChanged, LV_EVENT_VALUE_CHANGED, this);

    // Create a group so the encoder can navigate
    group = lv_group_create();
    lv_group_add_obj(group, brightnessSlider);
    lv_group_add_obj(group, volumeSlider);
    lv_group_add_obj(group, timezoneDropdown);
    lv_group_set_editing(group, true);
    lv_group_focus_obj(brightnessSlider);
    lv_indev_set_group(dial->encoder, group);
}

void SettingsApp::stop() {
    if (group) {
        lv_group_del(group);
        group = nullptr;
    }
    if (brightnessSlider) {
        lv_obj_del(brightnessSlider);
        brightnessSlider = nullptr;
    }
    if (volumeSlider) {
        lv_obj_del(volumeSlider);
        volumeSlider = nullptr;
    }
    if (timezoneDropdown) {
        lv_obj_del(timezoneDropdown);
        timezoneDropdown = nullptr;
    }
}

void SettingsApp::update() {
#ifdef ESP_PLATFORM
    // Could poll hardware if needed
#endif
}

void SettingsApp::onBrightnessChanged(lv_event_t *e) {
    auto self   = static_cast<SettingsApp *>(lv_event_get_user_data(e));
    auto slider = lv_event_get_target_obj(e);
    self->saveSettings();
#ifdef ESP_PLATFORM
    int val = lv_slider_get_value(slider);
    M5.Display.setBrightness(val);
#endif
}

void SettingsApp::onVolumeChanged(lv_event_t *e) {
    auto self   = static_cast<SettingsApp *>(lv_event_get_user_data(e));
    auto slider = lv_event_get_target_obj(e);
    self->saveSettings();
#ifdef ESP_PLATFORM
    int vol = lv_slider_get_value(slider);
    M5.Speaker.setVolume(vol);
#endif
}

void SettingsApp::onTimezoneChanged(lv_event_t *e) {
    auto self     = static_cast<SettingsApp *>(lv_event_get_user_data(e));
    auto dropdown = lv_event_get_target_obj(e);
    self->saveSettings();
#ifdef ESP_PLATFORM
    // Example approach: store numeric offset
    int selIndex = lv_dropdown_get_selected(dropdown);
    // Implementation depends on how you set the time zone in your environment
    // e.g. self->applySettings() later might use setenv(), tzset(), etc.
#endif
}

void SettingsApp::loadSettings() {
#ifdef ESP_PLATFORM
    prefs.begin("settings", true);
    int bright = prefs.getInt("brightness", 128);
    int vol    = prefs.getInt("volume", 50);
    int tzIdx  = prefs.getInt("timezone", 12); // default: UTC+0 line in the list
    prefs.end();

    if (brightnessSlider) lv_slider_set_value(brightnessSlider, bright, LV_ANIM_OFF);
    if (volumeSlider) lv_slider_set_value(volumeSlider, vol, LV_ANIM_OFF);
    if (timezoneDropdown) lv_dropdown_set_selected(timezoneDropdown, tzIdx);
#endif
}

void SettingsApp::saveSettings() {
#ifdef ESP_PLATFORM
    prefs.begin("settings", false);
    if (brightnessSlider) {
        prefs.putInt("brightness", lv_slider_get_value(brightnessSlider));
    }
    if (volumeSlider) {
        prefs.putInt("volume", lv_slider_get_value(volumeSlider));
    }
    if (timezoneDropdown) {
        prefs.putInt("timezone", lv_dropdown_get_selected(timezoneDropdown));
    }
    prefs.end();
#endif
}

void SettingsApp::applySettings() {
#ifdef ESP_PLATFORM
    // Appear offline for non-ESP platforms
    if (brightnessSlider) {
        M5.Display.setBrightness(lv_slider_get_value(brightnessSlider));
    }
    if (volumeSlider) {
        M5.Speaker.setVolume(lv_slider_get_value(volumeSlider));
    }
    // For time zone, adapt as needed:
    // setenv("TZ", "UTC+...", 1); tzset(); etc.
#endif
}

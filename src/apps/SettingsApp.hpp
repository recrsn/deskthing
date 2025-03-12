#ifndef SETTINGSAPP_HPP
#define SETTINGSAPP_HPP

#include "App.hpp"

#ifdef ESP_PLATFORM
#include <Preferences.h>
#endif

class SettingsApp final : public App {
public:
    explicit SettingsApp(lvgl_m5_dial_t *dial);
    void start(lv_obj_t *parent) override;
    void stop() override;
    void update() override;

private:
    lv_group_t *group             = nullptr;
    lv_obj_t *brightnessSlider    = nullptr;
    lv_obj_t *volumeSlider        = nullptr;
    lv_obj_t *timezoneDropdown    = nullptr;

#ifdef ESP_PLATFORM
    Preferences prefs;
#endif

    static void onBrightnessChanged(lv_event_t *e);
    static void onVolumeChanged(lv_event_t *e);
    static void onTimezoneChanged(lv_event_t *e);

    void loadSettings();
    void saveSettings();
    void applySettings();
};

#endif // SETTINGSAPP_HPP

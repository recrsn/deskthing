#ifndef SETTINGSAPP_HPP
#define SETTINGSAPP_HPP

#include "App.hpp"
#include <lvgl.h>

#ifdef ESP_PLATFORM
#include <Preferences.h>
#endif

// Forward declaration
class SettingsApp;

// =============== BrightnessFragment ===============
class BrightnessFragment : public lv_fragment_t {
public:
    static lv_fragment_class_t* getClass() {
        static lv_fragment_class_t cls;
        cls.constructor_cb = BrightnessFragment::constructor;
        cls.create_obj_cb = BrightnessFragment::createObj;
        cls.instance_size = sizeof(BrightnessFragment);
        return &cls;
    }

    static void constructor(lv_fragment_t* self, void* args);
    static lv_obj_t* createObj(lv_fragment_t* self, lv_obj_t* parent);
    static void onBrightnessChanged(lv_event_t* e);

    SettingsApp* settingsApp = nullptr;
    lv_obj_t* brightnessSlider = nullptr;
    lv_obj_t* valueLabel = nullptr;
};

// =============== VolumeFragment ===============
class VolumeFragment : public lv_fragment_t {
public:
    static lv_fragment_class_t* getClass() {
        static lv_fragment_class_t cls;
        cls.constructor_cb = VolumeFragment::constructor;
        cls.create_obj_cb = VolumeFragment::createObj;
        cls.instance_size = sizeof(VolumeFragment);
        return &cls;
    }

    static void constructor(lv_fragment_t* self, void* args);
    static lv_obj_t* createObj(lv_fragment_t* self, lv_obj_t* parent);
    static void onVolumeChanged(lv_event_t* e);

    SettingsApp* settingsApp = nullptr;
    lv_obj_t* volumeSlider = nullptr;
    lv_obj_t* valueLabel = nullptr;
};

// =============== TimezoneFragment ===============
class TimezoneFragment : public lv_fragment_t {
public:
    static lv_fragment_class_t* getClass() {
        static lv_fragment_class_t cls;
        cls.constructor_cb = TimezoneFragment::constructor;
        cls.create_obj_cb = TimezoneFragment::createObj;
        cls.instance_size = sizeof(TimezoneFragment);
        return &cls;
    }

    static void constructor(lv_fragment_t* self, void* args);
    static lv_obj_t* createObj(lv_fragment_t* self, lv_obj_t* parent);
    static void onTimezoneChanged(lv_event_t* e);

    SettingsApp* settingsApp = nullptr;
    lv_obj_t* timezoneDropdown = nullptr;
};

// =============== SettingsApp ===============
class SettingsApp final : public App {
public:
    explicit SettingsApp(lvgl_m5_dial_t* dial);
    void start(lv_obj_t* parent) override;
    void stop() override;
    void update() override;

    // Make prefs accessible to fragments
#ifdef ESP_PLATFORM
    Preferences prefs;
#endif

private:
    lv_group_t* group = nullptr;
    lv_obj_t* settingsList = nullptr;
    lv_obj_t* mainScreen = nullptr;
    
    // Fragment manager
    lv_fragment_manager_t fragmentManager;
    lv_fragment_t* currentFragment = nullptr;
    
    // Fragment classes
    lv_fragment_class_t* brightnessFragmentClass = nullptr;
    lv_fragment_class_t* volumeFragmentClass = nullptr;
    lv_fragment_class_t* timezoneFragmentClass = nullptr;

    static void onListItemClicked(lv_event_t* e);

    void loadSettings();
    void saveSettings();
    void applySettings();
};

#endif  // SETTINGSAPP_HPP

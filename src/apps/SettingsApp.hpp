#ifndef SETTINGSAPP_HPP
#define SETTINGSAPP_HPP
//
// #include <lvgl.h>
//
// #include "App.hpp"
//
// #ifdef ESP_PLATFORM
// #include <Preferences.h>
// #endif
//
// class SettingsApp final : public App {
// public:
//     explicit SettingsApp(lvgl_m5_dial_t* dial);
//     void start(lv_obj_t* parent) override;
//     void stop() override;
//     void update() override;
//
//     // Make prefs accessible to fragments
// #ifdef ESP_PLATFORM
//     Preferences prefs;
// #endif
//
// private:
//     lv_group_t* group      = nullptr;
//     lv_obj_t* settingsList = nullptr;
//     lv_obj_t* mainScreen   = nullptr;
//
//     lv_fragment_manager_t* fragmentManager;
//
//     lv_fragment_class_t* brightnessFragmentClass = nullptr;
//     lv_fragment_class_t* volumeFragmentClass     = nullptr;
//     lv_fragment_class_t* timezoneFragmentClass   = nullptr;
//
//     static void onListItemClicked(lv_event_t* e);
//     void loadSettings();
//     void saveSettings();
//     void applySettings();
//     friend static lv_obj_t* brightness_fragment_create_obj(lv_fragment_t* self, lv_obj_t* container);
//     friend static void brightness_fragment_constructor(lv_fragment_t* self, void* args);
// };
//
// typedef struct brightness_fragment_t {
//     lv_fragment_t base;
//     lv_obj_t* label;
//     int depth;
//     int counter;
// } brightness_fragment_t;
//
// static const lv_fragment_class_t sample_cls = {
//     .constructor_cb = SettingsApp::brightness_fragment_constructor,
//     .create_obj_cb  = SettingsApp::brightness_fragment_create_obj,
//     .instance_size  = sizeof(brightness_fragment_t),
// };
//
#endif  // SETTINGSAPP_HPP

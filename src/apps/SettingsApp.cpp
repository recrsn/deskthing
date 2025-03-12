#include "SettingsApp.hpp"

#include <lvgl.h>

#ifdef ESP_PLATFORM
#include <M5Unified.h>  // For M5.Display, M5.Speaker, etc.
#endif

// =============== BrightnessFragment Implementation ===============

void BrightnessFragment::constructor(lv_fragment_t* self, void* args) {
    lv_fragment_t::constructor(self, args);
    auto* instance = static_cast<BrightnessFragment*>(self);
    instance->settingsApp = static_cast<SettingsApp*>(args);
}

lv_obj_t* BrightnessFragment::createObj(lv_fragment_t* self, lv_obj_t* parent) {
    auto* instance = static_cast<BrightnessFragment*>(self);
    
    // Create container
    lv_obj_t* container = lv_obj_create(parent);
    lv_obj_set_size(container, 180, 180);
    lv_obj_center(container);
    
    // Create title
    lv_obj_t* title = lv_label_create(container);
    lv_label_set_text(title, "Brightness");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    
    // Create slider
    instance->brightnessSlider = lv_slider_create(container);
    lv_slider_set_range(instance->brightnessSlider, 0, 255);
    
    // Load current value
#ifdef ESP_PLATFORM
    instance->settingsApp->prefs.begin("settings", true);
    int bright = instance->settingsApp->prefs.getInt("brightness", 128);
    instance->settingsApp->prefs.end();
    lv_slider_set_value(instance->brightnessSlider, bright, LV_ANIM_OFF);
#else
    lv_slider_set_value(instance->brightnessSlider, 128, LV_ANIM_OFF);
#endif
    
    lv_obj_align(instance->brightnessSlider, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(instance->brightnessSlider, onBrightnessChanged, 
                        LV_EVENT_VALUE_CHANGED, instance);
    
    // Create value label
    instance->valueLabel = lv_label_create(container);
    lv_label_set_text_fmt(instance->valueLabel, "%d", 
                         lv_slider_get_value(instance->brightnessSlider));
    lv_obj_align(instance->valueLabel, LV_ALIGN_CENTER, 0, 30);
    
    return container;
}

void BrightnessFragment::onBrightnessChanged(lv_event_t* e) {
    auto* fragment = static_cast<BrightnessFragment*>(lv_event_get_user_data(e));
    auto* slider = lv_event_get_target_obj(e);
    int value = lv_slider_get_value(slider);
    
    // Update label
    lv_label_set_text_fmt(fragment->valueLabel, "%d", value);
    
    // Apply and save setting
#ifdef ESP_PLATFORM
    M5.Display.setBrightness(value);
    
    fragment->settingsApp->prefs.begin("settings", false);
    fragment->settingsApp->prefs.putInt("brightness", value);
    fragment->settingsApp->prefs.end();
#endif
}

// =============== VolumeFragment Implementation ===============

void VolumeFragment::constructor(lv_fragment_t* self, void* args) {
    lv_fragment_t::constructor(self, args);
    auto* instance = static_cast<VolumeFragment*>(self);
    instance->settingsApp = static_cast<SettingsApp*>(args);
}

lv_obj_t* VolumeFragment::createObj(lv_fragment_t* self, lv_obj_t* parent) {
    auto* instance = static_cast<VolumeFragment*>(self);
    
    // Create container
    lv_obj_t* container = lv_obj_create(parent);
    lv_obj_set_size(container, 180, 180);
    lv_obj_center(container);
    
    // Create title
    lv_obj_t* title = lv_label_create(container);
    lv_label_set_text(title, "Volume");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    
    // Create slider
    instance->volumeSlider = lv_slider_create(container);
    lv_slider_set_range(instance->volumeSlider, 0, 100);
    
    // Load current value
#ifdef ESP_PLATFORM
    instance->settingsApp->prefs.begin("settings", true);
    int vol = instance->settingsApp->prefs.getInt("volume", 50);
    instance->settingsApp->prefs.end();
    lv_slider_set_value(instance->volumeSlider, vol, LV_ANIM_OFF);
#else
    lv_slider_set_value(instance->volumeSlider, 50, LV_ANIM_OFF);
#endif
    
    lv_obj_align(instance->volumeSlider, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(instance->volumeSlider, onVolumeChanged, 
                        LV_EVENT_VALUE_CHANGED, instance);
    
    // Create value label
    instance->valueLabel = lv_label_create(container);
    lv_label_set_text_fmt(instance->valueLabel, "%d%%", 
                         lv_slider_get_value(instance->volumeSlider));
    lv_obj_align(instance->valueLabel, LV_ALIGN_CENTER, 0, 30);
    
    return container;
}

void VolumeFragment::onVolumeChanged(lv_event_t* e) {
    auto* fragment = static_cast<VolumeFragment*>(lv_event_get_user_data(e));
    auto* slider = lv_event_get_target_obj(e);
    int value = lv_slider_get_value(slider);
    
    // Update label
    lv_label_set_text_fmt(fragment->valueLabel, "%d%%", value);
    
    // Apply and save setting
#ifdef ESP_PLATFORM
    M5.Speaker.setVolume(value);
    
    fragment->settingsApp->prefs.begin("settings", false);
    fragment->settingsApp->prefs.putInt("volume", value);
    fragment->settingsApp->prefs.end();
#endif
}

// =============== TimezoneFragment Implementation ===============

void TimezoneFragment::constructor(lv_fragment_t* self, void* args) {
    lv_fragment_t::constructor(self, args);
    auto* instance = static_cast<TimezoneFragment*>(self);
    instance->settingsApp = static_cast<SettingsApp*>(args);
}

lv_obj_t* TimezoneFragment::createObj(lv_fragment_t* self, lv_obj_t* parent) {
    auto* instance = static_cast<TimezoneFragment*>(self);
    
    // Create container
    lv_obj_t* container = lv_obj_create(parent);
    lv_obj_set_size(container, 180, 180);
    lv_obj_center(container);
    
    // Create title
    lv_obj_t* title = lv_label_create(container);
    lv_label_set_text(title, "Timezone");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    
    // Create dropdown
    instance->timezoneDropdown = lv_dropdown_create(container);
    lv_dropdown_set_options(instance->timezoneDropdown,
                           "UTC-12\nUTC-11\nUTC-10\nUTC-9\nUTC-8\n"
                           "UTC-7\nUTC-6\nUTC-5\nUTC-4\nUTC-3\n"
                           "UTC-2\nUTC-1\nUTC+0\nUTC+1\nUTC+2\n"
                           "UTC+3\nUTC+4\nUTC+5\nUTC+6\nUTC+7\n"
                           "UTC+8\nUTC+9\nUTC+10\nUTC+11\nUTC+12");
    
    // Load current value
#ifdef ESP_PLATFORM
    instance->settingsApp->prefs.begin("settings", true);
    int tzIdx = instance->settingsApp->prefs.getInt("timezone", 12);  // default: UTC+0
    instance->settingsApp->prefs.end();
    lv_dropdown_set_selected(instance->timezoneDropdown, tzIdx);
#else
    lv_dropdown_set_selected(instance->timezoneDropdown, 12);  // default: UTC+0
#endif
    
    lv_obj_align(instance->timezoneDropdown, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(instance->timezoneDropdown, onTimezoneChanged, 
                        LV_EVENT_VALUE_CHANGED, instance);
    
    return container;
}

void TimezoneFragment::onTimezoneChanged(lv_event_t* e) {
    auto* fragment = static_cast<TimezoneFragment*>(lv_event_get_user_data(e));
    auto* dropdown = lv_event_get_target_obj(e);
    int tzIdx = lv_dropdown_get_selected(dropdown);
    
    // Save setting
#ifdef ESP_PLATFORM
    fragment->settingsApp->prefs.begin("settings", false);
    fragment->settingsApp->prefs.putInt("timezone", tzIdx);
    fragment->settingsApp->prefs.end();
    
    // Apply timezone setting
    // Implementation depends on how you set the time zone in your environment
    // e.g., setenv("TZ", "UTC+...", 1); tzset(); etc.
#endif
}

// =============== SettingsApp Implementation ===============

SettingsApp::SettingsApp(lvgl_m5_dial_t* dial) : App(dial) {
    // Initialize fragment manager
    lv_fragment_manager_init(&fragmentManager);
    
    // Register fragment classes
    brightnessFragmentClass = BrightnessFragment::getClass();
    volumeFragmentClass = VolumeFragment::getClass();
    timezoneFragmentClass = TimezoneFragment::getClass();
}

void SettingsApp::start(lv_obj_t* screen) {
    mainScreen = screen;
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), 0);
    
    // Create a list to hold our settings items
    settingsList = lv_list_create(screen);
    lv_obj_set_size(settingsList, 180, 180);
    lv_obj_align(settingsList, LV_ALIGN_CENTER, 0, 0);
    
    // Add each setting as a list item
    lv_obj_t* btnBrightness = lv_list_add_btn(settingsList, nullptr, "Brightness");
    lv_obj_add_event_cb(btnBrightness, onListItemClicked, LV_EVENT_CLICKED, this);
    
    lv_obj_t* btnVolume = lv_list_add_btn(settingsList, nullptr, "Volume");
    lv_obj_add_event_cb(btnVolume, onListItemClicked, LV_EVENT_CLICKED, this);
    
    lv_obj_t* btnTimezone = lv_list_add_btn(settingsList, nullptr, "Timezone");
    lv_obj_add_event_cb(btnTimezone, onListItemClicked, LV_EVENT_CLICKED, this);
    
    // Create navigation group
    group = lv_group_create();
    lv_group_add_obj(group, settingsList);
    lv_indev_set_group(dial->encoder, group);
    
    // Load settings
    loadSettings();
}

void SettingsApp::stop() {
    // Clean up fragment manager
    lv_fragment_manager_deinit(&fragmentManager);
    
    // Clean up group
    if (group) {
        lv_group_del(group);
        group = nullptr;
    }
}

void SettingsApp::update() {
    // Nothing to update regularly
}

void SettingsApp::onListItemClicked(lv_event_t* e) {
    auto* self = static_cast<SettingsApp*>(lv_event_get_user_data(e));
    lv_obj_t* btn = lv_event_get_target_obj(e);
    const char* txt = lv_list_get_button_text(self->settingsList, btn);
    
    // Hide the main list
    lv_obj_add_flag(self->settingsList, LV_OBJ_FLAG_HIDDEN);
    
    // Create back button
    lv_obj_t* btnBack = lv_btn_create(self->mainScreen);
    lv_obj_set_size(btnBack, 60, 30);
    lv_obj_align(btnBack, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_add_event_cb(btnBack, [](lv_event_t* e) {
        auto* self = static_cast<SettingsApp*>(lv_event_get_user_data(e));
        
        // Remove current fragment
        if (self->currentFragment) {
            lv_fragment_manager_remove(&self->fragmentManager, self->currentFragment);
            self->currentFragment = nullptr;
        }
        
        // Remove back button
        lv_obj_del(lv_event_get_target_obj(e));
        
        // Show the main list again
        lv_obj_clear_flag(self->settingsList, LV_OBJ_FLAG_HIDDEN);
        
        // Reset focus to the list
        lv_indev_set_group(self->dial->encoder, self->group);
        
    }, LV_EVENT_CLICKED, self);
    
    lv_obj_t* lblBack = lv_label_create(btnBack);
    lv_label_set_text(lblBack, "Back");
    lv_obj_center(lblBack);
    
    // Create appropriate fragment
    if (strcmp(txt, "Brightness") == 0) {
        self->currentFragment = lv_fragment_manager_create(&self->fragmentManager, 
                                                         self->brightnessFragmentClass, 
                                                         self);
    } else if (strcmp(txt, "Volume") == 0) {
        self->currentFragment = lv_fragment_manager_create(&self->fragmentManager, 
                                                         self->volumeFragmentClass, 
                                                         self);
    } else if (strcmp(txt, "Timezone") == 0) {
        self->currentFragment = lv_fragment_manager_create(&self->fragmentManager, 
                                                         self->timezoneFragmentClass, 
                                                         self);
    }
    
    // Add fragment to screen
    if (self->currentFragment) {
        lv_fragment_manager_add(&self->fragmentManager, self->currentFragment, self->mainScreen);
    }
}

void SettingsApp::loadSettings() {
#ifdef ESP_PLATFORM
    prefs.begin("settings", true);
    // Settings are loaded in each fragment's createObj method
    prefs.end();
#endif
}

void SettingsApp::saveSettings() {
    // Each fragment saves its own settings when values change
}

void SettingsApp::applySettings() {
    // Each fragment applies its own settings when values change
}

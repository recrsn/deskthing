#include "SettingsApp.hpp"

#include <lvgl.h>

#ifdef ESP_PLATFORM
#include <M5Unified.h>  // For M5.Display, M5.Speaker, etc.
#endif

void SettingsApp::createBrightnessFragment(lv_obj_t *oldScreen) {
    lv_obj_add_flag(oldScreen, LV_OBJ_FLAG_HIDDEN); // hide the list
    lv_obj_t *frag = lv_obj_create(lv_scr_act());
    lv_obj_set_size(frag, 180, 180);
    lv_obj_center(frag);

    brightnessSlider = lv_slider_create(frag);
    lv_slider_set_range(brightnessSlider, 0, 255);
    lv_slider_set_value(brightnessSlider, 128, LV_ANIM_OFF);
    lv_obj_align(brightnessSlider, LV_ALIGN_CENTER, 0, -20);
    lv_obj_add_event_cb(brightnessSlider, onBrightnessChanged, LV_EVENT_VALUE_CHANGED, this);

    // Save/Cancel buttons (placed below)
    lv_obj_t *btnSave = lv_btn_create(lv_scr_act());
    lv_obj_align(btnSave, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    lv_obj_add_event_cb(btnSave, [](lv_event_t *e) {
        auto self = static_cast<SettingsApp *>(lv_event_get_user_data(e));
        // Save changes, return to list
        self->saveSettings();
        lv_obj_del(lv_event_get_target_obj(e)); // remove button
        // cleanup or hide the fragment
    }, LV_EVENT_CLICKED, this);

    lv_obj_t *lblSave = lv_label_create(btnSave);
    lv_label_set_text(lblSave, "Save");
    lv_obj_center(lblSave);

    lv_obj_t *btnCancel = lv_btn_create(lv_scr_act());
    lv_obj_align(btnCancel, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_obj_add_event_cb(btnCancel, [](lv_event_t *e) {
        auto self = static_cast<SettingsApp *>(lv_event_get_user_data(e));
        // discard changes if needed, remove fragment
        // Show the main list again
    }, LV_EVENT_CLICKED, this);

    lv_obj_t *lblCancel = lv_label_create(btnCancel);
    lv_label_set_text(lblCancel, "Cancel");
    lv_obj_center(lblCancel);
}

void SettingsApp::createVolumeFragment(lv_obj_t *oldScreen) {
    lv_obj_add_flag(oldScreen, LV_OBJ_FLAG_HIDDEN); // hide the list
    lv_obj_t *frag = lv_obj_create(lv_scr_act());
    lv_obj_set_size(frag, 180, 180);
    lv_obj_center(frag);

    volumeSlider = lv_slider_create(frag);
    lv_slider_set_range(volumeSlider, 0, 100);
    lv_slider_set_value(volumeSlider, 50, LV_ANIM_OFF);
    lv_obj_align(volumeSlider, LV_ALIGN_CENTER, 0, -20);
    lv_obj_add_event_cb(volumeSlider, onVolumeChanged, LV_EVENT_VALUE_CHANGED, this);

    // Save/Cancel buttons (placed below)
    lv_obj_t *btnSave = lv_btn_create(lv_scr_act());
    lv_obj_align(btnSave, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    lv_obj_add_event_cb(btnSave, [](lv_event_t *e) {
        auto self = static_cast<SettingsApp *>(lv_event_get_user_data(e));
        // Save changes, return to list
        self->saveSettings();
        lv_obj_del(lv_event_get_target_obj(e)); // remove button
        // cleanup or hide the fragment
    }, LV_EVENT_CLICKED, this);

    lv_obj_t *lblSave = lv_label_create(btnSave);
    lv_label_set_text(lblSave, "Save");
    lv_obj_center(lblSave);

    lv_obj_t *btnCancel = lv_btn_create(lv_scr_act());
    lv_obj_align(btnCancel, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_obj_add_event_cb(btnCancel, [](lv_event_t *e) {
        auto self = static_cast<SettingsApp *>(lv_event_get_user_data(e));
        // discard changes if needed, remove fragment
        // Show the main list again
    }, LV_EVENT_CLICKED, this);

    lv_obj_t *lblCancel = lv_label_create(btnCancel);
    lv_label_set_text(lblCancel, "Cancel");
    lv_obj_center(lblCancel);
}

void SettingsApp::createTimezoneFragment(lv_obj_t *oldScreen) {
    lv_obj_add_flag(oldScreen, LV_OBJ_FLAG_HIDDEN); // hide the list
    lv_obj_t *frag = lv_obj_create(lv_scr_act());
    lv_obj_set_size(frag, 180, 180);
    lv_obj_center(frag);

    timezoneDropdown = lv_dropdown_create(frag);
    lv_dropdown_set_options(timezoneDropdown,
                            "UTC-12\nUTC-11\nUTC-10\nUTC-9\nUTC-8\n"
                            "UTC-7\nUTC-6\nUTC-5\nUTC-4\nUTC-3\n"
                            "UTC-2\nUTC-1\nUTC+0\nUTC+1\nUTC+2\n"
                            "UTC+3\nUTC+4\nUTC+5\nUTC+6\nUTC+7\n"
                            "UTC+8\nUTC+9\nUTC+10\nUTC+11\nUTC+12");
    lv_obj_align(timezoneDropdown, LV_ALIGN_CENTER, 0, -20);
    lv_obj_add_event_cb(timezoneDropdown, onTimezoneChanged, LV_EVENT_VALUE_CHANGED, this);

    // Save/Cancel buttons (placed below)
    lv_obj_t *btnSave = lv_btn_create(lv_scr_act());
    lv_obj_align(btnSave, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    lv_obj_add_event_cb(btnSave, [](lv_event_t *e) {
        auto self = static_cast<SettingsApp *>(lv_event_get_user_data(e));
        // Save changes, return to list
        self->saveSettings();
        lv_obj_del(lv_event_get_target_obj(e)); // remove button
        // cleanup or hide the fragment
    }, LV_EVENT_CLICKED, this);

    lv_obj_t *lblSave = lv_label_create(btnSave);
    lv_label_set_text(lblSave, "Save");
    lv_obj_center(lblSave);

    lv_obj_t *btnCancel = lv_btn_create(lv_scr_act());
    lv_obj_align(btnCancel, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_obj_add_event_cb(btnCancel, [](lv_event_t *e) {
        auto self = static_cast<SettingsApp *>(lv_event_get_user_data(e));
        // discard changes if needed, remove fragment
        // Show the main list again
    }, LV_EVENT_CLICKED, this);

    lv_obj_t *lblCancel = lv_label_create(btnCancel);
    lv_label_set_text(lblCancel, "Cancel");
    lv_obj_center(lblCancel);
}

SettingsApp::SettingsApp(lvgl_m5_dial_t *dial) : App(dial) {}

void SettingsApp::start(lv_obj_t *screen) {
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), 0);
    // Keep overall size ~180x180
    // Create a list to hold our settings items
    settingsList = lv_list_create(screen);
    lv_obj_set_size(settingsList, 180, 180);
    lv_obj_align(settingsList, LV_ALIGN_CENTER, 0, 0);

    // Add each setting as a list item
    lv_obj_t *btnBrightness = lv_list_add_btn(settingsList, nullptr, "Brightness");
    lv_obj_add_event_cb(btnBrightness, onListItemClicked, LV_EVENT_CLICKED, this);

    lv_obj_t *btnVolume = lv_list_add_btn(settingsList, nullptr, "Volume");
    lv_obj_add_event_cb(btnVolume, onListItemClicked, LV_EVENT_CLICKED, this);

    lv_obj_t *btnTimezone = lv_list_add_btn(settingsList, nullptr, "Timezone");
    lv_obj_add_event_cb(btnTimezone, onListItemClicked, LV_EVENT_CLICKED, this);
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

void SettingsApp::onListItemClicked(lv_event_t *e) {
    auto self = static_cast<SettingsApp *>(lv_event_get_user_data(e));
    lv_obj_t *btn = lv_event_get_target_obj(e);
    const char *txt = lv_list_get_button_text(self->settingsList, btn);

    // Create a child "fragment" screen
    // Hide the main list while child is displayed, or move it off-screen
    // Then show the relevant controls for that setting
    if (strcmp(txt, "Brightness") == 0) {
        self->createBrightnessFragment(lv_obj_get_parent(btn));
    } else if (strcmp(txt, "Volume") == 0) {
        self->createVolumeFragment(lv_obj_get_parent(btn));
    } else if (strcmp(txt, "Timezone") == 0) {
        self->createTimezoneFragment(lv_obj_get_parent(btn));
    }
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
    int tzIdx  = prefs.getInt("timezone", 12);  // default: UTC+0 line in the list
    prefs.end();

    if (brightnessSlider) {
        lv_slider_set_value(brightnessSlider, bright, LV_ANIM_OFF);
    }
    if (volumeSlider) {
        lv_slider_set_value(volumeSlider, vol, LV_ANIM_OFF);
    }
    if (timezoneDropdown) {
        lv_dropdown_set_selected(timezoneDropdown, tzIdx);
    }
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

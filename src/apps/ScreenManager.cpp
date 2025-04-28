#include "ScreenManager.hpp"

#include "../platform/log.h"
#include "../resources/fontawesome.h"

static const char* LOG_TAG = "ScreenManager";

void ScreenManager::backButtonEventHandler(lv_event_t* e) {
    auto* manager = static_cast<ScreenManager*>(lv_event_get_user_data(e));
    if (manager) {
        LOG_D(LOG_TAG, "Back button pressed, returning to launcher");
        manager->showLauncher();
    }
}

void ScreenManager::launcherEventHandler(lv_event_t* e) {
    auto* btn     = static_cast<lv_obj_t*>(lv_event_get_target(e));
    auto appName  = static_cast<const char*>(lv_obj_get_user_data(btn));
    auto* manager = static_cast<ScreenManager*>(lv_event_get_user_data(e));
    if (manager && appName) {
        LOG_I(LOG_TAG, "Launcher button clicked for app: %s", appName);
        manager->launchApp(appName);
    }
}

ScreenManager::ScreenManager(lvgl_m5_dial_t* dial) : dial(dial) {
    LOG_I(LOG_TAG, "Initializing ScreenManager");
    screen = lv_obj_create(nullptr);
}

void ScreenManager::registerApp(const std::string& name, const char* icon, std::function<std::unique_ptr<App>(lvgl_m5_dial_t*)> factory) {
    LOG_I(LOG_TAG, "Registering app: %s", name.c_str());
    appRegistry[name] = std::move(factory);
    appIcons[name] = icon;
}

void ScreenManager::addAppButton(lv_obj_t* appList, const std::string& appName) {
    const char* icon = LV_SYMBOL_IMAGE;
    if (appIcons.find(appName) != appIcons.end()) {
        icon = appIcons[appName];
    }
    
    lv_obj_t* btn = lv_list_add_btn(appList, icon, appName.c_str());
    lv_obj_set_style_text_font(btn, &fontawesome, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);
    lv_obj_set_height(btn, 48);
    lv_obj_set_user_data(btn, (void*)appName.c_str());
    lv_obj_add_event_cb(btn, launcherEventHandler, LV_EVENT_CLICKED, this);
}

void ScreenManager::showLauncher() {
    LOG_I(LOG_TAG, "Showing launcher");
    stopCurrentApp();
    auto newScreen = lv_obj_create(nullptr);

    lv_obj_t* list = lv_list_create(newScreen);
    lv_obj_set_style_border_width(list, 0, 0);        // Remove borders
    lv_obj_set_style_bg_opa(list, LV_OPA_TRANSP, 0);  // Set transparent background
    lv_obj_set_size(list, 180, 180);
    lv_obj_center(list);
    lv_obj_set_style_bg_color(newScreen, lv_color_hex(0x0), 0);

    LOG_D(LOG_TAG, "Adding app buttons to launcher");
    for (const auto& [name, _] : appRegistry) {
        addAppButton(list, name);
    }

    lv_scr_load(newScreen);
    lv_obj_del(screen);
    screen = newScreen;
}

void ScreenManager::launchApp(const std::string& appName) {
    if (appRegistry.find(appName) == appRegistry.end()) {
        LOG_E(LOG_TAG, "Failed to launch app: %s (not registered)", appName.c_str());
        return;
    }

    LOG_I(LOG_TAG, "Launching app: %s", appName.c_str());
    const auto newScreen = lv_obj_create(nullptr);

    currentApp = appRegistry[appName](dial);
    currentApp->start(newScreen);

    lv_scr_load(newScreen);
    lv_obj_del(screen);
    screen = newScreen;
    LOG_D(LOG_TAG, "App %s launched successfully", appName.c_str());
}

void ScreenManager::updateApp() const {
    if (currentApp) {
        currentApp->update();
    }
}

void ScreenManager::stopCurrentApp() {
    if (currentApp) {
        LOG_I(LOG_TAG, "Stopping current app");
        currentApp->stop();
        currentApp.reset();
    }
}

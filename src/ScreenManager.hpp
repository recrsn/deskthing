// ScreenManager.hpp
#ifndef SCREENMANAGER_HPP
#define SCREENMANAGER_HPP

#include <lvgl.h>

#include <functional>
#include <map>
#include <memory>

#include "apps/App.hpp"

class ScreenManager {
    lv_obj_t* screen;
    std::unique_ptr<App> currentApp;
    std::map<std::string, std::function<std::unique_ptr<App>(lvgl_m5_dial_t*)>> appRegistry;
    std::map<std::string, const char*> appIcons;
    lvgl_m5_dial_t* dial;

    static void backButtonEventHandler(lv_event_t* e);
    static void launcherEventHandler(lv_event_t* e);
    void addAppButton(lv_obj_t* appList, const std::string& appName);

public:
    explicit ScreenManager(lvgl_m5_dial_t* dial);
    void registerApp(const std::string& name, const char* icon, std::function<std::unique_ptr<App>(lvgl_m5_dial_t*)> factory);
    void showLauncher();
    void launchApp(const std::string& appName);
    void updateApp() const;

private:
    void stopCurrentApp();
};

#endif  // SCREENMANAGER_HPP

//
// Created by Amitosh Swain Mahapatra on 12/02/25.
//

#include "Launcher.h"
#include "lvgl_port_m5stack.hpp"

#include <misc/lv_event_private.h>

#define LV_SYMBOL_CLOCK "\xEF\x80\x81"

Launcher::Launcher(ScreenManager *manager) : App(manager) {
    launcherScreen = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(launcherScreen, lv_color_hex(0x000000), 0);

    lv_obj_t *appList = lv_list_create(launcherScreen);
    lv_obj_center(appList);

    /*Add buttons to the list*/
    /* Currently supported apps
     * Clock
     * Weather
     * Timer
     * Stopwatch
     * Radio
     * Light
     * Settings
     */

    lv_obj_t *btn;
    btn = lv_list_add_button(appList, LV_SYMBOL_IMAGE, "Clock");
    lv_obj_add_event_cb(
        btn,
        [](lv_event_t *event) {
            auto manager = static_cast<ScreenManager *>(lv_event_get_user_data(event));
            manager->setScreen("clock");
        },
        LV_EVENT_CLICKED, manager);

    btn = lv_list_add_button(appList, LV_SYMBOL_IMAGE, "Weather");
    lv_obj_add_event_cb(
        btn,
        [](lv_event_t *event) {
            auto manager = static_cast<ScreenManager *>(lv_event_get_user_data(event));
            manager->setScreen("weather");
        },
        LV_EVENT_CLICKED, manager);

    btn = lv_list_add_button(appList, LV_SYMBOL_IMAGE, "Timer");
    lv_obj_add_event_cb(
        btn,
        [](lv_event_t *event) {
            auto manager = static_cast<ScreenManager *>(lv_event_get_user_data(event));
            manager->setScreen("timer");
        },
        LV_EVENT_CLICKED, manager);

    btn = lv_list_add_button(appList, LV_SYMBOL_IMAGE, "Stopwatch");
    lv_obj_add_event_cb(
        btn,
        [](lv_event_t *event) {
            auto manager = static_cast<ScreenManager *>(lv_event_get_user_data(event));
            manager->setScreen("stopwatch");
        },
        LV_EVENT_CLICKED, manager);

    btn = lv_list_add_button(appList, LV_SYMBOL_IMAGE, "Radio");
    lv_obj_add_event_cb(
        btn,
        [](lv_event_t *event) {
            auto manager = static_cast<ScreenManager *>(lv_event_get_user_data(event));
            manager->setScreen("radio");
        },
        LV_EVENT_CLICKED, manager);

    btn = lv_list_add_button(appList, LV_SYMBOL_IMAGE, "Light");
    lv_obj_add_event_cb(
        btn,
        [](lv_event_t *event) {
            auto manager = static_cast<ScreenManager *>(lv_event_get_user_data(event));
            manager->setScreen("light");
        },
        LV_EVENT_CLICKED, manager);


    btn = lv_list_add_button(appList, LV_SYMBOL_IMAGE, "Settings");
    lv_obj_add_event_cb(
        btn,
        [](lv_event_t *event) {
            auto manager = static_cast<ScreenManager *>(lv_event_get_user_data(event));
            manager->setScreen("settings");
        },
        LV_EVENT_CLICKED, manager);
}

void Launcher::init() {
    if (lvgl_port_lock()) {
        lv_scr_load(launcherScreen);
        lvgl_port_unlock();
    }
}

Launcher::~Launcher() {
    if (lvgl_port_lock()) {
        lv_obj_del(launcherScreen);
        lvgl_port_unlock();
    }
}
//
// Created by Amitosh Swain Mahapatra on 11/02/25.
//
#include "lvgl_port_m5stack.hpp"
#include <ctime>
#include "ClockApp.h"

ClockApp::ClockApp(ScreenManager *manager) : App(manager) {
    clockScreen = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(clockScreen, lv_color_hex(0x000000), 0);

    // Time label
    timeLabel = lv_label_create(clockScreen);
    lv_obj_set_style_text_font(timeLabel, &lv_font_montserrat_40, 0);
    lv_obj_set_style_text_color(timeLabel, lv_color_hex(0xF0F0F0), 0);
    lv_obj_align(timeLabel, LV_ALIGN_TOP_MID, 0, 40);

    // Date label
    dateLabel = lv_label_create(clockScreen);
    lv_obj_set_style_text_font(dateLabel, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(dateLabel, lv_color_hex(0xF0F0F0), 0);
    lv_obj_align(dateLabel, LV_ALIGN_CENTER, 0, 0);

    // WiFi icon
    wifiIcon = lv_label_create(clockScreen);
    lv_label_set_text(wifiIcon, LV_SYMBOL_WIFI);
    lv_obj_set_style_text_color(wifiIcon, lv_color_hex(0x00FF00), 0);
    lv_obj_align(wifiIcon, LV_ALIGN_BOTTOM_MID, 0, -40);
    //        update();
}

void ClockApp::init() {
    if (lvgl_port_lock()) {
        lv_scr_load(clockScreen);
        lvgl_port_unlock();
    }
}

void ClockApp::update() {
    if (lvgl_port_lock()) {
        const time_t now = time(nullptr);
        const tm *t      = localtime(&now);

        static char timeStr[6];
        strftime(timeStr, sizeof(timeStr), "%H:%M", t);
        lv_label_set_text(timeLabel, timeStr);

        static char dateStr[20];
        strftime(dateStr, sizeof(dateStr), "%a, %d %b", t);
        lv_label_set_text(dateLabel, dateStr);

        lvgl_port_unlock();
    }
}

ClockApp::~ClockApp() {
    if (lvgl_port_lock()) {
        lv_obj_del(clockScreen);
        lvgl_port_unlock();
    }
}
//
// Created by Amitosh Swain Mahapatra on 11/02/25.
//
#include "ClockApp.hpp"

#include <lvgl.h>

#ifdef ESP_PLATFORM
#include <lwip/err.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <ctime>

#include "lvgl_port_m5stack.hpp"

void ClockApp::setBrightnessCallback(void *data) {
#ifdef BULB_IP
    auto arc        = static_cast<lv_obj_t *>(data);
    auto brightness = lv_arc_get_value(arc);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        return;
    }

    sockaddr_in servaddr = {};
    servaddr.sin_family  = AF_INET;
    servaddr.sin_port    = htons(38899);  // WiZ light UDP port
    inet_aton(BULB_IP, &servaddr.sin_addr);

    // Create JSON payload: {"method":"setPilot","params":{"dimming":brightness}}
    char payload[100];
    snprintf(payload, sizeof(payload), R"({"method":"setPilot","params":{"dimming":%d}})", brightness);

    sendto(sockfd, payload, strlen(payload), 0, (sockaddr *)&servaddr, sizeof(servaddr));
    close(sockfd);
#endif
}

void ClockApp::start(lv_obj_t *screen) {
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), 0);
    // Time label
    timeLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(timeLabel, &lv_font_montserrat_40, 0);
    lv_obj_align(timeLabel, LV_ALIGN_TOP_MID, 0, 40);

    // Date label
    dateLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(dateLabel, &lv_font_montserrat_18, 0);
    lv_obj_align(dateLabel, LV_ALIGN_CENTER, 0, 0);

    // WiFi icon
    wifiIcon = lv_label_create(screen);
    lv_label_set_text(wifiIcon, LV_SYMBOL_WIFI);
    lv_obj_add_flag(wifiIcon, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_set_style_text_color(wifiIcon, lv_color_hex(0x00FF00), 0);
    lv_obj_align(wifiIcon, LV_ALIGN_BOTTOM_MID, 0, -40);

    // brightness overlay
    brightnessOverlay = lv_obj_create(screen);
    lv_obj_remove_flag(brightnessOverlay, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(brightnessOverlay, LV_OBJ_FLAG_FLOATING);
    lv_obj_add_flag(brightnessOverlay, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_size(brightnessOverlay, LV_PCT(100), LV_PCT(100));
    lv_obj_center(brightnessOverlay);
    lv_obj_set_style_bg_color(brightnessOverlay, lv_color_hex(0x000000), 0);

    brightnessSlider = lv_arc_create(brightnessOverlay);
    lv_obj_set_size(brightnessSlider, 200, 200);
    lv_obj_center(brightnessSlider);
    lv_arc_set_value(brightnessSlider, 50);
    lv_obj_set_state(brightnessSlider, LV_STATE_FOCUSED, true);

    brightnessLabel = lv_label_create(brightnessOverlay);
    lv_obj_set_style_text_font(brightnessLabel, &lv_font_montserrat_40, 0);
    lv_label_set_text(brightnessLabel, "50%");
    lv_obj_center(brightnessLabel);

    lv_obj_t *title = lv_label_create(brightnessOverlay);
    lv_label_set_text(title, "Brightness");
    lv_obj_align_to(title, brightnessLabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    group = lv_group_create();
    lv_group_add_obj(group, brightnessSlider);
    lv_group_focus_obj(brightnessSlider);
    lv_group_set_editing(group, true);
    lv_group_focus_freeze(group, true);
    lv_indev_set_group(dial->encoder, group);

    lv_obj_add_event_cb(brightnessSlider, onBrightnessChangedCallback, LV_EVENT_VALUE_CHANGED, this);

    overlayTimer = lv_timer_create(
        [](lv_timer_t *timer) {
            const auto overlay = static_cast<lv_obj_t *>(lv_timer_get_user_data(timer));
            lv_obj_add_flag(overlay, LV_OBJ_FLAG_HIDDEN);
        },
        5000, brightnessOverlay);
    lv_timer_set_auto_delete(overlayTimer, false);
    lv_timer_pause(overlayTimer);

    clockTimer = lv_timer_create(updateTime, 1000, this);
    lv_timer_ready(clockTimer);
}

void ClockApp::onBrightnessChangedCallback(lv_event_t *e) {
    const auto app      = static_cast<ClockApp *>(lv_event_get_user_data(e));
    const lv_obj_t *arc = lv_event_get_target_obj(e);
    lv_label_set_text_fmt(app->brightnessLabel, "%" LV_PRId32 "%%", lv_arc_get_value(arc));

    if (lv_obj_has_flag(app->brightnessOverlay, LV_OBJ_FLAG_HIDDEN)) {
        lv_obj_clear_flag(app->brightnessOverlay, LV_OBJ_FLAG_HIDDEN);
    }
    lv_timer_reset(app->overlayTimer);
    lv_timer_resume(app->overlayTimer);

    // Send brightness command to Wiz light
    lv_async_call(setBrightnessCallback, (void *)arc);
}

void ClockApp::updateTime(lv_timer_t *timer) {
    const auto app   = static_cast<ClockApp *>(lv_timer_get_user_data(timer));
    const time_t now = time(nullptr);
    const tm *t      = localtime(&now);

    static char timeStr[6];
    strftime(timeStr, sizeof(timeStr), "%H:%M", t);
    lv_label_set_text(app->timeLabel, timeStr);

    static char dateStr[20];
    strftime(dateStr, sizeof(dateStr), "%a, %d %b", t);
    lv_label_set_text(app->dateLabel, dateStr);
}

void ClockApp::update() {
    if (lv_obj_has_flag(this->brightnessOverlay, LV_OBJ_FLAG_HIDDEN)) {
        lv_indev_data_t data;
        auto indev_get_read_cb = lv_indev_get_read_cb(this->dial->encoder);
        indev_get_read_cb(this->dial->encoder, &data);
        if (data.enc_diff) {
            lv_arc_set_value(this->brightnessSlider, lv_arc_get_value(this->brightnessSlider) + data.enc_diff);
            lv_obj_send_event(this->brightnessSlider, LV_EVENT_VALUE_CHANGED, nullptr);
            lv_obj_clear_flag(this->brightnessOverlay, LV_OBJ_FLAG_HIDDEN);
            lv_timer_reset(overlayTimer);
            lv_timer_resume(overlayTimer);
        }
    }
}

void ClockApp::stop() {
    if (overlayTimer) {
        lv_timer_del(overlayTimer);
    }

    if (clockTimer) {
        lv_timer_del(clockTimer);
    }

    if (group) {
        lv_group_del(group);
    }
}

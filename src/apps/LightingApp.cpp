//
// Created by Amitosh Swain Mahapatra on 07/03/25.
//

#include "LightingApp.hpp"

#ifndef ESP_PLATFORM
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#else
#include <lwip/sockets.h>
#endif

#include <util.h>

#include "resources/cct_arc.h"
#include "resources/fontawesome.h"
#include "resources/hsv_circle.h"

LightingApp::LightingApp(lvgl_m5_dial_t* d) : App(d) {
    udp_socket          = socket(AF_INET, SOCK_DGRAM, 0);
    wiz_addr.sin_family = AF_INET;
    wiz_addr.sin_port   = htons(38899);
    inet_aton(BULB_IP, &wiz_addr.sin_addr);
}

void LightingApp::start(lv_obj_t* screen) {
    container = lv_obj_create(screen);
    lv_obj_set_size(container, 240, 240);
    lv_obj_set_style_bg_color(container, lv_color_hex(0x000000), 0);
    lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLLABLE);

    arcCanvas = lv_image_create(container);
    lv_image_set_src(arcCanvas, &hsv_circle);
    lv_obj_center(arcCanvas);

    bulb_icon = lv_label_create(container);
    lv_label_set_text(bulb_icon, LV_SYMBOL_LIGHTBULB);
    lv_obj_set_style_text_font(bulb_icon, &fontawesome, 0);
    lv_obj_align(bulb_icon, LV_ALIGN_CENTER, 0, 0);

    cct_label = lv_label_create(container);
    lv_label_set_text_fmt(cct_label, "%dK", current_cct);
    lv_obj_align_to(cct_label, bulb_icon, LV_ALIGN_TOP_MID, 0, -20);
    lv_obj_add_flag(cct_label, LV_OBJ_FLAG_HIDDEN);

    arcSlider = lv_arc_create(container);
    lv_obj_set_size(arcSlider, 200, 200);
    lv_obj_center(arcSlider);
    lv_obj_add_event_cb(arcSlider, arc_event_cb, LV_EVENT_VALUE_CHANGED, this);
    lv_obj_set_style_arc_opa(arcSlider, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_arc_opa(arcSlider, LV_OPA_TRANSP, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(arcSlider, lv_color_hex(0xFFFFFF), LV_PART_KNOB);

    toggleBtn = lv_switch_create(container);
    lv_obj_set_size(toggleBtn, 80, 40);
    lv_obj_align_to(toggleBtn, bulb_icon, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_add_event_cb(toggleBtn, toggle_mode, LV_EVENT_VALUE_CHANGED, this);
    lv_obj_set_state(toggleBtn, LV_STATE_CHECKED, true);

    lv_obj_t* rgbLabel = lv_label_create(container);
    lv_label_set_text(rgbLabel, "RGB");
    lv_obj_set_style_text_font(rgbLabel, &lv_font_montserrat_12, 0);
    lv_obj_align_to(rgbLabel, toggleBtn, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    lv_obj_t* cctLabel = lv_label_create(container);
    lv_label_set_text(cctLabel, "CCT");
    lv_obj_set_style_text_font(cctLabel, &lv_font_montserrat_12, 0);
    lv_obj_align_to(cctLabel, toggleBtn, LV_ALIGN_OUT_LEFT_MID, -5, 0);

    group = lv_group_create();
    lv_group_add_obj(group, arcSlider);
    lv_group_focus_obj(arcSlider);
    lv_group_set_editing(group, true);
    lv_group_focus_freeze(group, true);
    lv_indev_set_group(dial->encoder, group);

    update_display();
}

void LightingApp::toggle_mode(lv_event_t* e) {
    auto* self        = static_cast<LightingApp*>(lv_event_get_user_data(e));
    self->is_rgb_mode = !self->is_rgb_mode;
    self->update_display();
}

void LightingApp::arc_event_cb(lv_event_t* e) {
    auto* self = static_cast<LightingApp*>(lv_event_get_user_data(e));
    if (self->is_rgb_mode) {
        self->current_color = lv_arc_get_value(self->arcSlider);
    } else {
        self->current_cct = lv_arc_get_value(self->arcSlider);
        lv_label_set_text_fmt(self->cct_label, "%dK", self->current_cct * 100);
    }
    self->send_udp_packet();
    self->update_display();
}

void LightingApp::update_display() const {
    if (this->is_rgb_mode) {
        lv_image_set_src(this->arcCanvas, &hsv_circle);
        lv_obj_add_flag(this->cct_label, LV_OBJ_FLAG_HIDDEN);
        lv_arc_set_range(this->arcSlider, 0, 360);
        lv_arc_set_angles(this->arcSlider, 0, 360);
        lv_arc_set_bg_angles(this->arcSlider, 0, 360);
        lv_arc_set_value(this->arcSlider, this->current_color);
    } else {
        lv_image_set_src(this->arcCanvas, &cct_arc);
        lv_obj_clear_flag(this->cct_label, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text_fmt(this->cct_label, "%dK", this->current_cct);
        lv_arc_set_range(this->arcSlider, 22, 65);
        lv_arc_set_angles(this->arcSlider, 135, 45);
        lv_arc_set_bg_angles(this->arcSlider, 135, 45);
        lv_arc_set_value(this->arcSlider, this->current_cct);
    }
}

void LightingApp::send_udp_packet() {
    char buffer[128];
    int len;
    if (is_rgb_mode) {
        auto [blue, green, red] = lv_color_hsv_to_rgb(this->current_color, 100, 100);
        len = snprintf(buffer, sizeof(buffer), R"({"method":"setPilot","params":{"r":%d,"g":%d,"b":%d}})", red, green,
                       blue);
    } else {
        len = snprintf(buffer, sizeof(buffer), R"({"method":"setPilot","params":{"temp":%d}})", current_cct * 100);
    }
    sendto(udp_socket, buffer, len, 0, reinterpret_cast<sockaddr*>(&wiz_addr), sizeof(wiz_addr));
}
void LightingApp::update() {}

void LightingApp::stop() {
    lv_obj_del(container);
    close(udp_socket);
}

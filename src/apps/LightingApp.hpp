//
// Created by Amitosh Swain Mahapatra on 07/03/25.
//

#ifndef LIGHTINGAPP_HPP
#define LIGHTINGAPP_HPP

#ifdef ESP_PLATFORM
#include <lwip/sockets.h>
#else
#include <netinet/in.h>
#endif

#include "App.hpp"
#include "lvgl.h"

class LightingApp : public App {
public:
    explicit LightingApp(lvgl_m5_dial_t* d);
    void start(lv_obj_t* screen) override;
    void stop() override;
    void update() override;

private:
    lv_obj_t* container  = nullptr;
    lv_obj_t* arcCanvas  = nullptr;
    lv_obj_t* arcSlider  = nullptr;
    lv_obj_t* toggleBtn  = nullptr;
    lv_obj_t* bulb_icon  = nullptr;
    lv_obj_t* cct_label  = nullptr;
    int current_color    = 0;
    int current_cct      = 31;
    bool is_rgb_mode     = true;
    int udp_socket       = -1;
    lv_color_t* buffer   = nullptr;
    lv_group_t* group    = nullptr;
    sockaddr_in wiz_addr = {};

    static void toggle_mode(lv_event_t* e);
    void send_udp_packet();
    static void arc_event_cb(lv_event_t* e);
    void update_display() const;
};

#endif  // LIGHTINGAPP_HPP

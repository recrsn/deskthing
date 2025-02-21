//
// Created by Amitosh Swain Mahapatra on 12/02/25.
//

// Timer.cpp
#include "Timer.h"
#include <lvgl.h>
#include <lvgl_port_m5stack.hpp>
#include <misc/lv_event_private.h>

#if defined(ARDUINO) && defined(ESP_PLATFORM)
#include <M5Dial.h>
#else
#define delay(x) (usleep(x))
#endif

Timer::Timer(ScreenManager *manager) : App(manager) {
    scr = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), 0);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    // Create a label for the timer

    hoursLabel = lv_label_create(scr);
    lv_obj_add_flag(hoursLabel, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_label_set_text(hoursLabel, "00");
    lv_obj_set_style_text_color(hoursLabel, lv_color_hex(0xC0C0C0), 0);
    lv_obj_set_style_text_color(hoursLabel, lv_color_hex(0xFFFFFF), LV_STATE_FOCUSED);
    lv_obj_set_size(hoursLabel, LV_SIZE_CONTENT, 50);
    lv_obj_set_style_text_font(hoursLabel, &lv_font_montserrat_40, 0);

    minutesLabel = lv_label_create(scr);
    lv_obj_add_flag(minutesLabel, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_label_set_text(minutesLabel, ":00:");
    lv_obj_set_style_text_color(minutesLabel, lv_color_hex(0xC0C0C0), 0);
    lv_obj_set_style_text_color(minutesLabel, lv_color_hex(0xFFFFFF), LV_STATE_FOCUSED);
    lv_obj_set_size(minutesLabel, LV_SIZE_CONTENT, 50);
    lv_obj_set_style_text_font(minutesLabel, &lv_font_montserrat_40, 0);
    lv_obj_add_event_cb(
        minutesLabel,
        [](lv_event_t *event) {
            const auto timer = static_cast<Timer *>(lv_event_get_user_data(event));
            if (event->code == LV_EVENT_CLICKED) {
                lv_obj_clear_state(timer->minutesLabel, LV_STATE_FOCUSED);
                lv_obj_clear_state(timer->hoursLabel, LV_STATE_FOCUSED);
                lv_obj_add_state(static_cast<lv_obj_t *>(event->current_target), LV_STATE_FOCUSED);
            }
        },
        LV_EVENT_CLICKED, this);

    secondsLabel = lv_label_create(scr);
    lv_obj_add_flag(secondsLabel, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_label_set_text(secondsLabel, "00");
    lv_obj_set_style_text_color(secondsLabel, lv_color_hex(0xC0C0C0), 0);
    lv_obj_set_style_text_color(secondsLabel, lv_color_hex(0xFFFFFF), LV_STATE_FOCUSED);
    lv_obj_set_size(secondsLabel, LV_SIZE_CONTENT, 50);
    lv_obj_set_style_text_font(secondsLabel, &lv_font_montserrat_40, 0);

    lv_obj_align(minutesLabel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align_to(hoursLabel, minutesLabel, LV_ALIGN_OUT_LEFT_MID, 0, 0);
    lv_obj_align_to(secondsLabel, minutesLabel, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

    // Create a start/stop button
    btnStartStop = lv_btn_create(scr);
    lv_obj_set_style_radius(btnStartStop, 0, 0);
    lv_obj_set_style_bg_color(btnStartStop, lv_color_hex(0x00FF00), 0);
    lv_obj_set_size(btnStartStop, LV_PCT(100), 40);
    lv_obj_align_to(btnStartStop, minutesLabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

    labelStartStop = lv_label_create(btnStartStop);
    lv_label_set_text(labelStartStop, "Start");
    lv_obj_align(labelStartStop, LV_ALIGN_CENTER, 0, 0);

    // Create a reset button
    btnReset = lv_btn_create(scr);
    lv_obj_set_style_radius(btnReset, 0, 0);
    lv_obj_set_size(btnReset, LV_PCT(100), 40);
    lv_obj_align_to(btnReset, btnStartStop, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    labelReset = lv_label_create(btnReset);
    lv_label_set_text(labelReset, "Reset");
    lv_obj_align(labelReset, LV_ALIGN_CENTER, 0, 0);

    // Create an LED
    led = lv_led_create(scr);
    lv_obj_set_size(led, 8, 8);
    // do not show the LED by default
    lv_led_off(led);

    lv_obj_align(led, LV_ALIGN_TOP_MID, 0, 10);
}

void Timer::init() {
    if (lvgl_port_lock()) {
        lv_scr_load(scr);
        lvgl_port_unlock();
    }
}

// void Timer::draw() {
//     if (mode == 3) {
//         M5.Speaker.tone(4000, 100);
//         delay(100);
//         z = !z;
//         if (z)
//             img.fillSprite(WHITE);
//         else
//             img.fillSprite(BLACK);
//     }
//
//     static char str[10];
//     snprintf(str, 10, "%02d:%02d:%02d", num[0], num[1], num[2]);
//     lv_label_set_text(label, str);
//
//     if (mode == 1) {
//         lv_led_on(led);
//     } else {
//         lv_led_off(led);
//     }
//
//     if (mode == 3) {
//         img.fillSprite(WHITE);
//     }
// }void Timer::reset() {
//     mode = 0;
//     num[2] = 15;
//     num[1] = 0;
//     num[0] = 0;
//     lv_label_set_text(label_start_stop, "Start");
// }
//
// void Timer::update() {
//     M5.update();
//
//     if (lv_btn_get_state(btnStartStop) == LV_BTN_STATE_PRESSED) {
//         if (mode == 0) {
//             mode = 1;
//             lv_label_set_text(label_start_stop, "Stop");
//         } else if (mode == 1) {
//             mode = 0;
//             lv_label_set_text(label_start_stop, "Start");
//         }
//         delay(200);
//     }
//
//     if (lv_btn_get_state(btn_reset) == LV_BTN_STATE_PRESSED) {
//         reset();
//         delay(200);
//     }
//
//     if (mode == 1) {
//         updateTime();
//     }
//
//     draw();
// }
//
// void Timer::draw() {
//     if (mode == 3) {
//         M5.Speaker.tone(4000, 100);
//         delay(100);
//         z = !z;
//         if (z)
//             img.fillSprite(WHITE);
//         else
//             img.fillSprite(BLACK);
//         static char str[10];
//         snprintf(str, 10, "%02d:%02d:%02d", num[0], num[1], num[2]);
//
//         img.drawString(str, 120, 120, 7);
//     } else {
//         img.fillSprite(BLACK);
//         img.fillRect(0, 160, 240, 60, 0x0A2D);
//         img.fillRect(0, 0, 240, 80, 0x0A2D);
//         img.setTextColor(WHITE, 0x0A2D);
//         img.drawString("START", 120, 190, 4);
//
//         img.setTextColor(ORANGE, BLACK);
//         img.drawString("RESET", 120, 232, 2);
//         // img.setTextColor(ORANGE, 0x0A2D);
//         // img.drawString("M5DIAL TIMER", 120, 60, 4);
//         img.setTextColor(WHITE, BLACK);
//         if (mode == 0) {
//             img.fillRect(14 + (chosen * 76), 150, 59, 4, GREEN);
//         }
//
//         static char str[10];
//         snprintf(str, 10, "%02d:%02d:%02d", num[0], num[1], num[2]);
//         img.drawString(str, 120, 120, 7);
//     }
//
//     img.pushSprite(0, 0);
// }
//
// void Timer::updateTime() {
//     if (num[0] <= 0 && num[1] <= 0 && num[2] <= 0 && mode == 1) mode = 3;
//
//     auto dt = M5.Rtc.getDateTime();
//     if (lastS != dt.time.seconds) {
//         num[2]--;
//         lastS = dt.time.seconds;
//
//         if (num[2] < 0) {
//             num[1]--;
//             num[2] = mm[2] - 1;
//         }
//         if (num[1] < 0) {
//             num[0]--;
//             num[1] = mm[1] - 1;
//         }
//     }
// }
//
// void Timer::reset() {
//     mode   = 0;
//     num[2] = 15;
//     num[1] = 0;
//     num[0] = 0;
// }
//
// void Timer::update() {
//     M5.update();
//
//     if (M5.BtnA.isPressed()) {
//         M5.Speaker.tone(2800, 100);
//         // reset();
//         delay(200);
//     }
//
//     if (mode == 0) {
//         auto t = M5.Touch.getDetail();
//         if (t.isPressed()) {
//             if (deb == 0) {
//                 deb = 1;
//                 M5.Speaker.tone(3000, 100);
//                 if (t.y > 160) {
//                     mode = 1;
//                 };
//                 if (t.y > 86 && t.y < 150) {
//                     if (t.x > 10 && t.x < 90) chosen = 0;
//                     if (t.x > 90 && t.x < 166) chosen = 1;
//                     if (t.x > 166 && t.x < 224) chosen = 2;
//                 }
//             }
//         } else {
//             deb = 0;
//         }
//
// #ifdef M5DIAL_H
//         long newPosition = M5Dial.Encoder.read();
//         if (newPosition != oldPosition) {
//             M5.Speaker.tone(3600, 30);
//             if (newPosition > oldPosition)
//                 num[chosen]++;
//             else
//                 num[chosen]--;
//             if (num[chosen] == mm[chosen]) num[chosen] = 0;
//             if (num[chosen] < 0) num[chosen] = mm[chosen] - 1;
//             oldPosition = newPosition;
//         }
// #endif
//     }
//
//     if (mode == 1) {
//         updateTime();
//     }
//
//     draw();
// }

Timer::~Timer() {
    if (lvgl_port_lock()) {
        lv_obj_del(scr);
        lvgl_port_unlock();
    }
}
//
// Created by Amitosh Swain Mahapatra on 12/02/25.
//

// Timer.cpp

#include "Timer.hpp"

#include <lvgl.h>

#include <M5Unified.hpp>
#include <cstdio>

#include "lvgl_port_m5stack.hpp"

#if defined(ARDUINO) && defined(ESP_PLATFORM)
#include <M5Dial.h>
#else
#define delay(x) (usleep(x * 1000))
#endif

#define TIMER_MAX (99 * 3600 + 59 * 60 + 59)

void Timer::start(lv_obj_t *parent) {
    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);

    hoursLabel = lv_label_create(parent);
    lv_obj_add_flag(hoursLabel, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(hoursLabel, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_label_set_text(hoursLabel, "00");
    lv_obj_set_style_text_color(hoursLabel, lv_color_hex(0x888888), 0);
    lv_obj_set_style_text_color(hoursLabel, lv_color_hex(0xFFFFFF), LV_STATE_FOCUSED);
    lv_obj_set_size(hoursLabel, LV_SIZE_CONTENT, 50);
    lv_obj_set_style_text_font(hoursLabel, &lv_font_montserrat_40, 0);

    lv_obj_add_event_cb(
        hoursLabel,
        [](lv_event_t *event) {
            const auto code = lv_event_get_code(event);

            auto key = lv_event_get_key(event);
            if (!key) {
                return;
            }
            const auto timer = static_cast<Timer *>(lv_event_get_user_data(event));
            if (key == LV_KEY_RIGHT) {
                if (timer->countdownTime < TIMER_MAX) {
                    timer->countdownTime += 3600;
                }
            } else if (key == LV_KEY_LEFT) {
                if (timer->countdownTime >= 3600) {
                    timer->countdownTime -= 3600;
                }
            }
            timer->updateTimer();
        },
        LV_EVENT_KEY, this);

    minutesLabel = lv_label_create(parent);
    lv_obj_add_flag(minutesLabel, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(minutesLabel, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_label_set_text(minutesLabel, ":00:");
    lv_obj_set_style_text_color(minutesLabel, lv_color_hex(0x888888), 0);
    lv_obj_set_style_text_color(minutesLabel, lv_color_hex(0xFFFFFF), LV_STATE_FOCUSED);
    lv_obj_set_size(minutesLabel, LV_SIZE_CONTENT, 50);
    lv_obj_set_style_text_font(minutesLabel, &lv_font_montserrat_40, 0);
    lv_obj_add_event_cb(
        minutesLabel,
        [](lv_event_t *event) {
            const auto app = static_cast<Timer *>(lv_event_get_user_data(event));
            auto key       = lv_event_get_key(event);
            if (!key) {
                return;
            }
            if (key == LV_KEY_RIGHT) {
                if (app->countdownTime < TIMER_MAX) {
                    app->countdownTime += 60;
                }
            } else if (key == LV_KEY_LEFT) {
                if (app->countdownTime >= 60) {
                    app->countdownTime -= 60;
                }
            }
            app->updateTimer();
        },
        LV_EVENT_ALL, this);

    secondsLabel = lv_label_create(parent);
    lv_obj_add_flag(secondsLabel, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(secondsLabel, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_label_set_text(secondsLabel, "00");
    lv_obj_set_style_text_color(secondsLabel, lv_color_hex(0x888888), 0);
    lv_obj_set_style_text_color(secondsLabel, lv_color_hex(0xFFFFFF), LV_STATE_FOCUSED);
    lv_obj_set_size(secondsLabel, LV_SIZE_CONTENT, 50);
    lv_obj_set_style_text_font(secondsLabel, &lv_font_montserrat_40, 0);

    lv_obj_add_event_cb(
        secondsLabel,
        [](lv_event_t *event) {
            const auto app = static_cast<Timer *>(lv_event_get_user_data(event));
            auto key       = lv_event_get_key(event);
            if (!key) {
                return;
            }
            if (key == LV_KEY_RIGHT) {
                if (app->countdownTime < TIMER_MAX) {
                    app->countdownTime += 1;
                }
            } else if (key == LV_KEY_LEFT) {
                if (app->countdownTime >= 1) {
                    app->countdownTime -= 1;
                }
            }
            app->updateTimer();
        },
        LV_EVENT_ALL, this);

    lv_obj_align(minutesLabel, LV_ALIGN_CENTER, 0, -25);
    lv_obj_align_to(hoursLabel, minutesLabel, LV_ALIGN_OUT_LEFT_MID, 0, 0);
    lv_obj_align_to(secondsLabel, minutesLabel, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

    // Create a start/stop button
    btnStartStop = lv_btn_create(parent);
    lv_obj_set_style_radius(btnStartStop, 0, 0);
    lv_obj_set_style_bg_color(btnStartStop, lv_color_hex(0x00CC22), 0);
    lv_obj_set_size(btnStartStop, LV_PCT(100), 50);
    lv_obj_align_to(btnStartStop, minutesLabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

    labelStartStop = lv_label_create(btnStartStop);
    lv_label_set_text(labelStartStop, "Start");
    lv_obj_align(labelStartStop, LV_ALIGN_CENTER, 0, 0);

    lv_obj_add_event_cb(
        btnStartStop,
        [](lv_event_t *event) {
            auto app = static_cast<Timer *>(lv_event_get_user_data(event));

            if (app->timerRunning) {
                app->stopTimer();
            } else {
                app->startTimer();
            }
        },
        LV_EVENT_CLICKED, this);

    // Create a reset button
    btnReset = lv_btn_create(parent);
    lv_obj_set_style_radius(btnReset, 0, 0);
    lv_obj_set_size(btnReset, LV_PCT(100), 50);
    lv_obj_align_to(btnReset, btnStartStop, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    lv_obj_add_event_cb(
        btnReset,
        [](lv_event_t *event) {
            auto app = static_cast<Timer *>(lv_event_get_user_data(event));
            app->reset();
        },
        LV_EVENT_CLICKED, this);

    labelReset = lv_label_create(btnReset);
    lv_label_set_text(labelReset, "Reset");
    lv_obj_align(labelReset, LV_ALIGN_CENTER, 0, 0);

    // Create an LED
    led = lv_led_create(parent);
    lv_led_set_color(led, lv_color_hex(0xff0000));
    lv_obj_set_size(led, 8, 8);
    // do not show the LED by default
    lv_led_off(led);

    lv_obj_align(led, LV_ALIGN_TOP_MID, 0, 10);

    timerGroup = lv_group_create();
    lv_group_add_obj(timerGroup, secondsLabel);
    lv_group_add_obj(timerGroup, minutesLabel);
    lv_group_add_obj(timerGroup, hoursLabel);

    lv_group_set_editing(timerGroup, true);  // Start in edit mode
    lv_group_set_wrap(timerGroup, false);    // Disable wrap-around
    lv_group_set_focus_cb(timerGroup, [](lv_group_t *group) {
        // automatically switch to edit mode when the group is focused
        lv_group_set_editing(group, true);
    });

    lv_indev_set_group(dial->encoder, timerGroup);

    // Timer to update the display
    timer = lv_timer_create(
        [](lv_timer_t *timer) {
            const auto app = static_cast<Timer *>(lv_timer_get_user_data(timer));
            if (app->timerRunning) {
                if (app->countdownTime > 0) {
                    app->countdownTime--;
                    app->updateTimer();
                } else {
                    app->timerElapsed();
                }
            }
        },
        1000, this);
    lv_timer_set_auto_delete(timer, false);
    lv_timer_pause(timer);

    toneTimer = lv_timer_create(
        [](lv_timer_t *timer) {
            auto app = static_cast<Timer *>(lv_timer_get_user_data(timer));
            lv_led_toggle(app->led);
            M5.Speaker.tone(2000, 50);
        },
        100, this);
    lv_timer_set_auto_delete(toneTimer, false);
    lv_timer_set_repeat_count(toneTimer, 4);
    lv_timer_pause(toneTimer);
}

void Timer::timerElapsed() {
    lv_timer_set_repeat_count(toneTimer, 4);
    lv_timer_reset(toneTimer);
    lv_timer_resume(toneTimer);
}

void Timer::updateTimer() const {
    int hours   = countdownTime / 3600;
    int minutes = (countdownTime % 3600) / 60;
    int seconds = countdownTime % 60;

    char timeStr[5];
    snprintf(timeStr, sizeof(timeStr), "%02d", hours);
    lv_label_set_text(hoursLabel, timeStr);

    snprintf(timeStr, sizeof(timeStr), ":%02d:", minutes);
    lv_label_set_text(minutesLabel, timeStr);

    snprintf(timeStr, sizeof(timeStr), "%02d", seconds);
    lv_label_set_text(secondsLabel, timeStr);
}

void Timer::startTimer() {
    if (countdownTime <= 0) {
        return;
    }
    timerRunning = true;
    lv_led_on(led);
    lv_label_set_text(labelStartStop, "Stop");
    lv_obj_set_style_bg_color(btnStartStop, lv_color_hex(0xCC0000), 0);
    lv_timer_resume(timer);
}

void Timer::stopTimer() {
    lv_led_off(led);
    lv_timer_pause(timer);
    lv_label_set_text(labelStartStop, "Start");
    lv_obj_set_style_bg_color(btnStartStop, lv_color_hex(0x00CC22), 0);
    timerRunning = false;
}

void Timer::reset() {
    countdownTime = 0;
    updateTimer();
    stop();
}

void Timer::stop() {
    lv_group_delete(timerGroup);
    lv_timer_delete(timer);
    lv_timer_delete(toneTimer);
    lv_obj_delete(hoursLabel);
    lv_obj_delete(minutesLabel);
    lv_obj_delete(secondsLabel);
    lv_obj_delete(btnStartStop);
    lv_obj_delete(labelStartStop);
    lv_obj_delete(btnReset);
    lv_obj_delete(labelReset);
    lv_obj_delete(led);
}

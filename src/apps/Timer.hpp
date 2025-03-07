//
// Created by Amitosh Swain Mahapatra on 12/02/25.
//

#ifndef TIMER_H
#define TIMER_H

#include <lvgl.h>

#include "App.hpp"

class Timer : public App {
public:
    explicit Timer(lvgl_m5_dial_t* dial) : App(dial) {}
    void start(lv_obj_t* parent) override;
    void stop() override;

private:
    int32_t countdownTime = 0;

    lv_obj_t* hoursLabel   = nullptr;
    lv_obj_t* minutesLabel = nullptr;
    lv_obj_t* secondsLabel = nullptr;

    lv_obj_t* btnStartStop             = nullptr;
    lv_obj_t* labelStartStop           = nullptr;
    lv_obj_t* btnReset                 = nullptr;
    lv_obj_t* labelReset               = nullptr;
    lv_obj_t* led                      = nullptr;
    lv_group_t* timerGroup             = nullptr;
    bool timerRunning                  = false;
    time_t lastUpdate                  = 0;
    lv_timer_t* timer                  = nullptr;
    lv_timer_t* toneTimer              = nullptr;
    lv_event_cb_t encoderEventCallback = nullptr;
    void buildGui();
    void timerElapsed();
    void updateTimer() const;
    void startTimer();
    void stopTimer();
    void reset();
};

#endif  // TIMER_H

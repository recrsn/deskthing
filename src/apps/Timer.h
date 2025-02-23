//
// Created by Amitosh Swain Mahapatra on 12/02/25.
//

#ifndef TIMER_H
#define TIMER_H

#include <lvgl.h>
#include "App.h"

class Timer : public App {
   public:
    explicit Timer(ScreenManager* manager);
    void stop();
    void reset();
    void init() override;
    ~Timer() override;
    ;

   private:
    int32_t countdownTime = 0;

    lv_obj_t* scr = nullptr;

    lv_obj_t* hoursLabel   = nullptr;
    lv_obj_t* minutesLabel = nullptr;
    lv_obj_t* secondsLabel = nullptr;

    lv_obj_t* btnStartStop   = nullptr;
    lv_obj_t* labelStartStop = nullptr;
    lv_obj_t* btnReset       = nullptr;
    lv_obj_t* labelReset     = nullptr;
    lv_obj_t* led            = nullptr;
    lv_group_t* timerGroup   = nullptr;
    bool timerRunning        = false;
    time_t lastUpdate        = 0;
    lv_timer_t* timer        = nullptr;
    lv_timer_t* toneTimer    = nullptr;
    void buildGui();
    void timerElapsed();
    void updateTimer() const;
    void start();
};

#endif  // TIMER_H

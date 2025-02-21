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
    void init() override;
    // void update() override;
    ~Timer() override;

   private:
    // void draw();
    // void updateTime();
    // void reset();

    int8_t hours   = 0;
    int8_t minutes = 0;
    int8_t seconds = 0;

    lv_obj_t* scr;

    lv_obj_t* hoursLabel;
    lv_obj_t* minutesLabel;
    lv_obj_t* secondsLabel;

    lv_obj_t* btnStartStop;
    lv_obj_t* labelStartStop;
    lv_obj_t* btnReset;
    lv_obj_t* labelReset;
    lv_obj_t* led;
};

#endif  // TIMER_H

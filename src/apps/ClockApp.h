//
// Created by Amitosh Swain Mahapatra on 11/02/25.
//

#ifndef CLOCKAPP_H
#define CLOCKAPP_H

#include "App.h"
#include <lvgl.h>

class ClockApp : public App {
   public:
    explicit ClockApp(ScreenManager *manager);
    void init() override;
    void update() override;
    ~ClockApp() override;

   private:
    lv_obj_t *clockScreen;
    lv_obj_t *timeLabel;
    lv_obj_t *dateLabel;
    lv_obj_t *wifiIcon;

    void createClockScreen();
};

#endif  // CLOCKAPP_H

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
    lv_obj_t *clockScreen = nullptr;
    lv_obj_t *timeLabel = nullptr;
    lv_obj_t *dateLabel = nullptr;
    lv_obj_t *wifiIcon = nullptr;

    void buildGui();
};

#endif  // CLOCKAPP_H

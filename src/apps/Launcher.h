//
// Created by Amitosh Swain Mahapatra on 12/02/25.
//

#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <lvgl.h>
#include "App.h"


class Launcher: public App {
public:
    explicit Launcher(ScreenManager *manager);
    void init() override;
    ~Launcher() override;

private:
    lv_obj_t *launcherScreen;
};



#endif //LAUNCHER_H

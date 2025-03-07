//
// Created by Amitosh Swain Mahapatra on 11/02/25.
//

#ifndef APP_HPP
#define APP_HPP

#include <lvgl.h>

#include "lvgl_port_m5stack.hpp"

class App {
protected:
    lvgl_m5_dial_t* dial;

public:
    explicit App(lvgl_m5_dial_t* d) : dial(d) {}
    virtual ~App()                       = default;
    virtual void start(lv_obj_t* parent) = 0;
    virtual void stop()                  = 0;
    virtual void update() {};
};

#endif  // APP_H

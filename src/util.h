
//
// Created by Amitosh Swain Mahapatra on 18/02/25.
//

#ifndef UTIL_H
#define UTIL_H

#include <lvgl.h>

#if defined(ARDUINO) && defined(ESP_PLATFORM)
#include <esp_system.h>
#endif

void generatePassword(char* password, int length);
lv_indev_t* findEncoder();
lv_color_t rgb888_to_565(lv_color_t rgb888);
#endif  // UTIL_H

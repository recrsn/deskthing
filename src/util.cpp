//
// Created by Amitosh Swain Mahapatra on 18/02/25.
//

#include "util.h"

#include <cstdint>
#include <random>

void generatePassword(char *password, const int length) {
    constexpr char charset[]     = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    constexpr size_t charsetSize = sizeof(charset) - 1;

    for (int i = 0; i < length; i++) {
        uint8_t randomIndex = random() % charsetSize;
        password[i]         = charset[randomIndex];
    }
    password[length] = '\0';
}

lv_indev_t *findEncoder() {
    lv_indev_t *indev = lv_indev_get_next(nullptr);
    while (indev) {
        if (lv_indev_get_type(indev) == LV_INDEV_TYPE_ENCODER) {
            return indev;
        }
        indev = lv_indev_get_next(indev);
    }
    return nullptr;
}
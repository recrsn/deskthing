//
// Created by Amitosh Swain Mahapatra on 18/02/25.
//

#include "util.h"

#include <cstdint>
#include <cstdlib>

void generatePassword(char *password, int length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const size_t charsetSize = sizeof(charset) - 1;

    for (int i = 0; i < length; i++) {
        uint8_t randomIndex = random() % charsetSize;
        password[i] = charset[randomIndex];
    }
    password[length] = '\0';
}
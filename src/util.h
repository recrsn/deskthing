
//
// Created by Amitosh Swain Mahapatra on 18/02/25.
//

#ifndef UTIL_H
#define UTIL_H

#if defined(ARDUINO) && defined(ESP_PLATFORM)
#include <esp_system.h>
#endif

void generatePassword(char *password, int length);

#endif  // UTIL_H

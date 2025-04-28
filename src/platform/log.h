#ifndef PLATFORM_LOG_H
#define PLATFORM_LOG_H

#ifdef ESP_PLATFORM
#include <esp_log.h>
#define LOG_I(tag, format, ...) ESP_LOGI(tag, format, ##__VA_ARGS__)
#define LOG_W(tag, format, ...) ESP_LOGW(tag, format, ##__VA_ARGS__)
#define LOG_E(tag, format, ...) ESP_LOGE(tag, format, ##__VA_ARGS__)
#define LOG_D(tag, format, ...) ESP_LOGD(tag, format, ##__VA_ARGS__)
#define LOG_V(tag, format, ...) ESP_LOGV(tag, format, ##__VA_ARGS__)
#else
#include <cstdio>
#define LOG_I(tag, format, ...) printf("[INFO][%s] " format "\n", tag, ##__VA_ARGS__)
#define LOG_W(tag, format, ...) printf("[WARN][%s] " format "\n", tag, ##__VA_ARGS__)
#define LOG_E(tag, format, ...) printf("[ERROR][%s] " format "\n", tag, ##__VA_ARGS__)
#define LOG_D(tag, format, ...) printf("[DEBUG][%s] " format "\n", tag, ##__VA_ARGS__)
#define LOG_V(tag, format, ...) printf("[VERBOSE][%s] " format "\n", tag, ##__VA_ARGS__)
#endif

#endif

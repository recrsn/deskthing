#include <M5Unified.h>
#include <lvgl.h>
#include <resources/fontawesome.h>

#include <apps/ClockApp.hpp>
#include <apps/LightingApp.hpp>
#include <apps/Timer.hpp>

#include "ScreenManager.hpp"
#include "lvgl_port_m5stack.hpp"

#if defined(ARDUINO) && defined(ESP_PLATFORM)
#include <M5Dial.h>
#include <WiFiManager.h>

#include "util.h"
#else
#include <unistd.h>
#define delay(x) (usleep(x * 1000))
#endif

ScreenManager *manager;

void setup() {
    m5::M5Unified::config_t cfg;
    // cfg.fallback_board = m5::board_t::board_M5Dial;
#ifdef ESP_PLATFORM
    cfg.serial_baudrate = 115200;
    M5Dial.begin(cfg, true, false);
#else
    M5.begin(cfg);
#endif
    M5.Rtc.setDateTime({{2023, 10, 25}, {15, 56, 56}});
    M5.Display.setBrightness(24);

#if defined(ARDUINO) && defined(ESP_PLATFORM)
#ifndef NO_WIFI
#if defined(TEST_WIFI_SSID) && defined(TEST_WIFI_PASS)
    WiFi.begin(TEST_WIFI_SSID, TEST_WIFI_PASS);
#else
    WiFiManager wm;
    char password[9];
    generatePassword(password, 8);
    char apName[] = "DeskThing";

    wm.setAPCallback([&apName, &password](WiFiManager *) {
        M5.Display.clear();
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.setTextDatum(middle_center);

        String qrString = "WIFI:T:WPA;S:" + String(apName) + ";P:" + String(password) + ";;";

        M5.Display.qrcode(qrString.c_str(), (M5.Display.width() - 100) / 2, (M5.Display.height() - 100) / 2, 100, 6);
        M5.Display.setTextDatum(bottom_center);
        M5.Display.drawString("Scan to Configure WiFi", M5.Display.width() / 2, 50);
        M5.Display.drawString("SSID: " + String(apName), M5.Display.width() / 2, 180);
        M5.Display.drawString("Password: " + String(password), M5.Display.width() / 2, 200);
    });

    if (!wm.autoConnect(apName, password)) {
        M5.Display.clear();
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.setTextDatum(middle_center);
        M5.Display.drawString("WiFi Setup Failed", M5.Display.width() / 2, M5.Display.height() / 2);
        delay(3000);
        ESP.restart();
    }
#endif
    // set time from NTP
    configTime(0, 0, "pool.ntp.org", "time.google.com", "time.windows.com");

#ifdef TZ
    setenv("TZ", TZ, true);
    tzset();
#endif
#endif
#endif
    lvgl_m5_dial_t *m5Dial = lvgl_port_init(M5.Display);

    if (m5Dial == nullptr) {
        LV_LOG_ERROR("Could not initialize display");
        M5.Display.fillScreen(TFT_RED);
        M5.Display.setColor(255, 255, 255);
        M5.Display.drawString("Could not initialize display", 0, 100);
        return;
    }

    if (lvgl_port_lock()) {
        manager = new ScreenManager(m5Dial);

        /* Currently supported apps
         * Clock
         * Timer
         */

        manager->registerApp("Clock", LV_SYMBOL_CLOCK, [](lvgl_m5_dial_t *d) { return std::make_unique<ClockApp>(d); });
        manager->registerApp("Timer", LV_SYMBOL_TIMER, [](lvgl_m5_dial_t *d) { return std::make_unique<Timer>(d); });
        manager->registerApp("Lighting", LV_SYMBOL_LIGHTBULB,
                             [](lvgl_m5_dial_t *d) { return std::make_unique<LightingApp>(d); });
        /* Apps to be added
         * Weather
         * Timer
         * Stopwatch
         * Radio
         * Light
         * Settings
         */

        manager->showLauncher();
        lvgl_port_unlock();
    }
}

void loop() {
#if defined(ARDUINO) && defined(ESP_PLATFORM)
    M5Dial.update();
    if (M5.BtnA.wasPressed()) {
        manager->showLauncher();
    }
#endif
    if (lvgl_port_lock()) {
        manager->updateApp();
        lvgl_port_unlock();
    }
    delay(10);
}

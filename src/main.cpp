#include <M5Unified.h>
#include <lvgl.h>
#include <resources/fontawesome.h>

// Include platform abstractions
#include "platform/Network.hpp"
#include "platform/log.h"

static const char* LOG_TAG = "Main";

#if defined(ESP_PLATFORM)
#include <M5Dial.h>

#if not defined(TEST_WIFI_SSID) && not defined(TEST_WIFI_PASSWORD)
#include <WiFiManager.h>
#endif
#else
#include <unistd.h>
#define delay(x) (usleep(x * 1000))
#endif

#include "apps/ClockApp.hpp"
#include "apps/LightingApp.hpp"
#include "apps/ScreenManager.hpp"
#include "apps/Timer.hpp"
#include "lvgl_port_m5stack.hpp"
#include "util.h"

ScreenManager *manager;

void setup() {
    LOG_I(LOG_TAG, "Starting DeskBit application");
    
    m5::M5Unified::config_t cfg;
    // cfg.fallback_board = m5::board_t::board_M5Dial;
#ifdef ESP_PLATFORM
    LOG_I(LOG_TAG, "Initializing ESP32 platform");
    cfg.serial_baudrate = 115200;
    M5Dial.begin(cfg, true, false);
#else
    LOG_I(LOG_TAG, "Initializing desktop platform");
    M5.begin(cfg);
#endif
    M5.Rtc.setDateTime({{2023, 10, 25}, {15, 56, 56}});
    M5.Display.setBrightness(24);

#if defined(ARDUINO) && defined(ESP_PLATFORM)
#ifndef NO_WIFI
#if defined(TEST_WIFI_SSID) && defined(TEST_WIFI_PASSWORD)
    LOG_I(LOG_TAG, "Connecting to WiFi using predefined credentials");
    WiFi.begin(TEST_WIFI_SSID, TEST_WIFI_PASSWORD);
#else
    LOG_I(LOG_TAG, "Starting WiFi Manager for configuration");
    WiFiManager wm;
    char password[9];
    generatePassword(password, 8);
    char apName[] = "DeskThing";
    LOG_I(LOG_TAG, "Created AP with name: %s, password: %s", apName, password);

    wm.setAPCallback([&apName, &password](WiFiManager *) {
        LOG_I(LOG_TAG, "WiFi AP mode active, showing QR code for configuration");
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
        LOG_E(LOG_TAG, "WiFi setup failed, restarting device");
        M5.Display.clear();
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.setTextDatum(middle_center);
        M5.Display.drawString("WiFi Setup Failed", M5.Display.width() / 2, M5.Display.height() / 2);
        delay(3000);
        ESP.restart();
    } else {
        LOG_I(LOG_TAG, "WiFi connected successfully");
    }
#endif
    // set time from NTP
    LOG_I(LOG_TAG, "Synchronizing time from NTP servers");
    configTime(0, 0, "pool.ntp.org", "time.google.com", "time.windows.com");

#ifdef TZ
    LOG_I(LOG_TAG, "Setting timezone to %s", TZ);
    setenv("TZ", TZ, true);
    tzset();
#endif
#endif
#endif
    LOG_I(LOG_TAG, "Initializing LVGL port");
    lvgl_m5_dial_t *m5Dial = lvgl_port_init(M5.Display);

    if (m5Dial == nullptr) {
        LOG_E(LOG_TAG, "Failed to initialize LVGL display");
        LV_LOG_ERROR("Could not initialize display");
        M5.Display.fillScreen(TFT_RED);
        M5.Display.setColor(255, 255, 255);
        M5.Display.drawString("Could not initialize display", 0, 100);
        return;
    }
    
    LOG_I(LOG_TAG, "LVGL display initialized successfully");

    if (lvgl_port_lock()) {
        LOG_I(LOG_TAG, "Initializing ScreenManager");
        manager = new ScreenManager(m5Dial);

        /* Currently supported apps
         * Clock
         * Timer
         */

        manager->registerApp("Clock", LV_SYMBOL_CLOCK, [](lvgl_m5_dial_t *d) { return std::make_unique<ClockApp>(d); });
        manager->registerApp("Timer", LV_SYMBOL_TIMER, [](lvgl_m5_dial_t *d) { return std::make_unique<Timer>(d); });
        manager->registerApp("Lighting", LV_SYMBOL_LIGHTBULB,
                             [](lvgl_m5_dial_t *d) { return std::make_unique<LightingApp>(d); });
        // manager->registerApp("Settings", LV_SYMBOL_SETTINGS, [](lvgl_m5_dial_t *d) { return
        // std::make_unique<SettingsApp>(d); });

        /* Apps to be added
         * Weather
         * Timer
         * Stopwatch
         * Radio
         * Light
         * Settings
         */

        LOG_I(LOG_TAG, "Starting Clock app");
        manager->launchApp("Clock");
        lvgl_port_unlock();
        LOG_I(LOG_TAG, "Setup complete");
    } else {
        LOG_E(LOG_TAG, "Failed to acquire LVGL port lock");
    }
}

void loop() {
#if defined(ARDUINO) && defined(ESP_PLATFORM)
    M5Dial.update();
    if (M5.BtnA.wasPressed()) {
        LOG_D(LOG_TAG, "Home button pressed");
        manager->launchApp("Clock");
    }
#endif
    if (lvgl_port_lock()) {
        manager->updateApp();
        lvgl_port_unlock();
    }

    // Handle display sleep/wake based on inactivity
    uint32_t inactiveTime = lv_display_get_inactive_time(nullptr);
    if (inactiveTime > 3600000) {
        static bool reported = false;
        if (!reported) {
            LOG_I(LOG_TAG, "Display inactive for %u ms, sleeping", inactiveTime);
            reported = true;
        }
        M5.Display.sleep();
    } else {
        static bool wasSleeping = false;
        if (wasSleeping) {
            LOG_I(LOG_TAG, "Waking display from sleep");
            wasSleeping = false;
        }
        M5.Display.wakeup();
    }

    delay(10);
}
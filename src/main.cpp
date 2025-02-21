#include <unistd.h>
#include "lvgl_port_m5stack.hpp"
#include "ScreenManager.h"
#include <M5Unified.h>
#include <apps/ClockApp.h>
#include <util.h>
#include <apps/Launcher.h>
#include <apps/Timer.h>

#if defined(ARDUINO) && defined(ESP_PLATFORM)
#include <M5Dial.h>
#include <WiFiManager.h>
#endif

LV_FONT_DECLARE(fontawesome)

M5GFX gfx;
ScreenManager* manager;

void setup() {
#ifdef M5DIAL_H
    m5::M5Unified::config_t cfg;
    cfg.serial_baudrate = 115200;
    M5Dial.begin(cfg, true, false);
#else
    M5.begin();
#endif

    M5.Rtc.setDateTime({{2023, 10, 25}, {15, 56, 56}});
    M5.Display.setBrightness(24);

    gfx.init();

#if defined(ARDUINO) && defined(ESP_PLATFORM)
    WiFiManager wm;
    char password[9];
    generatePassword(password, 8);
    char apName[] = "DeskThing";

    wm.setAPCallback([&apName, &password](WiFiManager*) {
        M5.Display.clear();
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.setTextDatum(middle_center);

        String qrString = "WIFI:T:WPA;S:" + String(apName) + ";P:" + String(password) + ";;";

        M5.Display.qrcode(qrString.c_str(), (M5.Display.width() - 120) / 2, (M5.Display.height() - 120) / 2, 100, 6);
        M5.Display.setTextDatum(bottom_center);
        M5.Display.drawString("Scan to Configure WiFi", M5.Display.width() / 2, 50);
    });

    if (!wm.autoConnect(apName, password)) {
        M5.Display.clear();
        M5.Display.setTextColor(TFT_WHITE);
        M5.Display.setTextDatum(middle_center);
        M5.Display.drawString("WiFi Setup Failed", M5.Display.width() / 2, M5.Display.height() / 2);
        delay(3000);
        ESP.restart();
    }

    // set time from NTP
    configTime(0, 0, "pool.ntp.org", "time.google.com", "time.windows.com");

    // set timezone, currently hardcoded to IST
    setenv("TZ", "IST-5:30", true);
    tzset();
#endif

    // set time from NTP

    lvgl_port_init(gfx);

    manager = new ScreenManager();
    manager->addScreen("launcher", new Launcher(manager));
    manager->addScreen("clock", new ClockApp(manager));
    manager->addScreen("timer", new Timer(manager));

    manager->setScreen("launcher");
}

void loop() {
#if defined(ARDUINO) && defined(ESP_PLATFORM)
    delay(10);
#elif !defined(ARDUINO) && (__has_include(<SDL2/SDL.h>) || __has_include(<SDL.h>))
    usleep(1000);
#endif

    manager->update();
}

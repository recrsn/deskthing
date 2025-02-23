//
// Created by Amitosh Swain Mahapatra on 18/02/25.
//

#include "ScreenManager.h"
#include <cstdio>
#include <unistd.h>

#include <apps/App.h>

void ScreenManager::addScreen(const std::string& name, App* screen) {
    screens[name] = screen;
}

void ScreenManager::setHomeScreen(const std::string& screenName) {
    homeScreen = screenName;
}

void ScreenManager::update() const {
    if (currentScreen != nullptr) {
        currentScreen->update();
    }
}

void ScreenManager::home() {
    setScreen(homeScreen);
}

void ScreenManager::setScreen(const std::string& screenName) {
    if (screens.find(screenName) == screens.end()) {
        printf("Screen not found %s\n", screenName.c_str());
        return;
    }

    delete currentScreen;

    currentScreen = screens[screenName];
    currentScreen->init();
}



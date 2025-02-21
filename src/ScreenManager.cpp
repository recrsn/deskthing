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

void ScreenManager::update() {
    if (currentScreen != nullptr) {
        currentScreen->update();
    }
}

void ScreenManager::setScreen(const std::string& screenName) {
    if (!screens.contains(screenName)) {
        printf("Screen not found %s\n", screenName.c_str());
        return;
    }

    delete currentScreen;

    currentScreen = screens[screenName];
    currentScreen->init();
}



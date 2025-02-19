//
// Created by Amitosh Swain Mahapatra on 18/02/25.
//

#include "ScreenManager.h"

ScreenManager::ScreenManager(std::map<std::string, App*> screens) {
    this->screens = screens;
}

void ScreenManager::update() {
    if (currentScreen != nullptr) {
        currentScreen->update();
    }
}

void ScreenManager::setScreen(std::string screenName) {
    if(screens.find(screenName) == screens.end()) {
        return;
    }

    currentScreen = screens[screenName];
    currentScreen->init();
}



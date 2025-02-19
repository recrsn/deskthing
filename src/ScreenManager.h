//
// Created by Amitosh Swain Mahapatra on 18/02/25.
//

#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include <map>
#include <string>
#include "apps/App.h"


class ScreenManager {
public:
    ScreenManager(std::map<std::string, App*> screens);
    void init();
    void update();
    void setScreen(std::string screenName);
private:
    std::map<std::string, App*> screens;
    App* currentScreen;
};



#endif //SCREENMANAGER_H

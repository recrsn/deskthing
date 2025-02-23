//
// Created by Amitosh Swain Mahapatra on 18/02/25.
//

#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include <map>
#include <string>

class App;
class ScreenManager {
   public:
    void addScreen(const std::string& name, App* screen);
    void setHomeScreen(const std::string& screenName);
    void update() const;
    void home();
    void setScreen(const std::string& screenName);

   private:
    std::map<std::string, App*> screens = {};
    App* currentScreen                  = nullptr;
    std::string homeScreen;
};

#endif  // SCREENMANAGER_H

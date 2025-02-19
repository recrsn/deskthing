//
// Created by Amitosh Swain Mahapatra on 11/02/25.
//

#ifndef APP_H
#define APP_H
#include <ScreenManager.h>

class App {
   public:
    explicit App(ScreenManager* manager) : manager(manager) {
    }
    virtual ~App()      = default;
    virtual void init() = 0;
    virtual void update() {
    }
protected:
  ScreenManager* manager;
};

#endif  // APP_H

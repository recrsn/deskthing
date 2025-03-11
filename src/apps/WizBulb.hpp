//
// Created by Amitosh Swain Mahapatra on 2024.
//

#ifndef WIZBULB_HPP
#define WIZBULB_HPP

#ifdef ESP_PLATFORM
#include <lwip/sockets.h>
#else
#include <netinet/in.h>
#endif

class WizBulb {
public:
    WizBulb();
    ~WizBulb();

    void setBrightness(int brightness);
    void setColor(int red, int green, int blue);
    void setColorTemperature(int temperature);

private:
    int udp_socket = -1;
    sockaddr_in wiz_addr = {};
    
    void sendCommand(const char* payload, int len);
};

#endif // WIZBULB_HPP

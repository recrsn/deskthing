//
// Created by Amitosh Swain Mahapatra on 2024.
//

#include "WizBulb.hpp"

#ifndef ESP_PLATFORM
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#else
#include <lwip/sockets.h>
#endif

#include <cstring>

WizBulb::WizBulb() {
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    
    wiz_addr.sin_family = AF_INET;
    wiz_addr.sin_port = htons(38899);  // WiZ light UDP port
    
#ifdef BULB_IP
    inet_aton(BULB_IP, &wiz_addr.sin_addr);
#endif
}

WizBulb::~WizBulb() {
    if (udp_socket >= 0) {
        close(udp_socket);
    }
}

void WizBulb::setBrightness(int brightness) {
    if (udp_socket < 0) return;
    
    char payload[100];
    int len = snprintf(payload, sizeof(payload), R"({"method":"setPilot","params":{"dimming":%d}})", brightness);
    
    sendCommand(payload, len);
}

void WizBulb::setColor(int red, int green, int blue) {
    if (udp_socket < 0) return;
    
    char payload[128];
    int len = snprintf(payload, sizeof(payload), 
                      R"({"method":"setPilot","params":{"r":%d,"g":%d,"b":%d}})", 
                      red, green, blue);
    
    sendCommand(payload, len);
}

void WizBulb::setColorTemperature(int temperature) {
    if (udp_socket < 0) return;
    
    char payload[100];
    int len = snprintf(payload, sizeof(payload), 
                      R"({"method":"setPilot","params":{"temp":%d}})", 
                      temperature);
    
    sendCommand(payload, len);
}

void WizBulb::sendCommand(const char* payload, int len) {
    if (udp_socket < 0) return;
    
    sendto(udp_socket, payload, len, 0, 
           reinterpret_cast<sockaddr*>(&wiz_addr), sizeof(wiz_addr));
}

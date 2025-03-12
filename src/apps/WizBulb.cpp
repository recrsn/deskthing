//
// Created by Amitosh Swain Mahapatra on 2024.
//

#include "WizBulb.hpp"

#ifndef ESP_PLATFORM
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#else
#include <lwip/sockets.h>
#endif

#include <ArduinoJson.h>
#include <cstring>

WizBulb::WizBulb() {
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (udp_socket >= 0) {
        setNonBlocking(udp_socket);
    }

    wiz_addr.sin_family = AF_INET;
    wiz_addr.sin_port   = htons(38899);  // WiZ light UDP port

#ifdef BULB_IP
    inet_aton(BULB_IP, &wiz_addr.sin_addr);
#endif

    // Initialize receive buffer
    receiveBuffer.resize(1024);
    
    // Initialize timeout tracking
    lastResponseTime = std::chrono::steady_clock::now();
}

WizBulb::~WizBulb() {
    if (udp_socket >= 0) {
        close(udp_socket);
    }
}

void WizBulb::setNonBlocking(int sock) {
#ifndef ESP_PLATFORM
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);
#else
    // ESP32 lwip sockets are non-blocking by default when using Arduino framework
    // If using ESP-IDF directly, you might need to set non-blocking mode
#endif
}

void WizBulb::setStateCallback(StateCallback callback) {
    stateCallback = std::move(callback);
}

void WizBulb::requestState() {
    if (udp_socket < 0) {
        return;
    }

    char payload[50];
    const int len = snprintf(payload, sizeof(payload), R"({"method":"getPilot"})");

    sendCommand(payload, len);
    
    // Update last response time
    lastResponseTime = std::chrono::steady_clock::now();
    timeoutOccurred = false;
}

bool WizBulb::isOnline() const {
    return state.isOnline;
}

const WizBulbState& WizBulb::getState() const {
    return state;
}

void WizBulb::setTimeout(unsigned long ms) {
    timeoutMs = ms;
}

void WizBulb::checkTimeout() {
    if (timeoutOccurred) {
        return; // Already timed out, waiting for next request
    }
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastResponseTime).count();
    
    if (elapsed > timeoutMs) {
        // Mark bulb as offline
        state.isOnline = false;
        timeoutOccurred = true;
        
        // Notify via callback
        if (stateCallback) {
            stateCallback(state);
        }
    }
}

void WizBulb::processStateResponse(const std::string& response) {
    // Mark the bulb as online since we received a response
    state.isOnline = true;
    timeoutOccurred = false;
    
    // Update last response time
    lastResponseTime = std::chrono::steady_clock::now();

    // Parse the JSON response using ArduinoJson
    JsonDocument doc;  // Adjust size as needed

    DeserializationError error = deserializeJson(doc, response);
    if (error) {
        // JSON parsing failed
        if (stateCallback) {
            stateCallback(state);
        }
        return;
    }

    // Get the result object
    const JsonObject result = doc["result"];
    if (result.isNull()) {
        if (stateCallback) {
            stateCallback(state);
        }
        return;
    }

    // Get the state value
    if (result["state"].is<bool>()) {
        state.isOn = result["state"].as<bool>();
    }

    // Get the dimming (brightness) value
    if (result["dimming"].is<int>()) {
        state.brightness = result["dimming"].as<int>();
    }

    // Check for RGB color values
    const bool hasRGB = result["r"].is<int>() && result["g"].is<int>() && result["b"].is<int>();
    if (hasRGB) {
        state.red       = result["r"].as<int>();
        state.green     = result["g"].as<int>();
        state.blue      = result["b"].as<int>();
        state.isRGBMode = true;
    }

    // Check for color temperature
    if (result["temp"].is<int>()) {
        state.temperature = result["temp"].as<int>();
        state.isRGBMode   = false;
    }

    // Notify via callback
    if (stateCallback) {
        stateCallback(state);
    }
}

void WizBulb::update() {
    // Check for timeout
    checkTimeout();
    
    // Check for incoming data
    if (udp_socket < 0) {
        return;
    }
    
    // Try to receive data
    sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);
    
    int received = recvfrom(udp_socket, receiveBuffer.data(), receiveBuffer.size() - 1, 0,
                           reinterpret_cast<sockaddr*>(&from_addr), &from_len);
    
    if (received > 0) {
        // Null-terminate the received data
        receiveBuffer[received] = '\0';
        
        // Process the response
        processStateResponse(std::string(receiveBuffer.data(), received));
    }
    // No need to handle EAGAIN/EWOULDBLOCK as this is expected for non-blocking sockets
}

void WizBulb::setBrightness(int brightness) {
    if (udp_socket < 0) {
        return;
    }

    char payload[100];
    int len = snprintf(payload, sizeof(payload), R"({"method":"setPilot","params":{"dimming":%d}})", brightness);

    sendCommand(payload, len);

    // Update local state
    state.brightness = brightness;
}

void WizBulb::setColor(int red, int green, int blue) {
    if (udp_socket < 0) {
        return;
    }

    char payload[128];
    int len = snprintf(payload, sizeof(payload), R"({"method":"setPilot","params":{"r":%d,"g":%d,"b":%d}})", red, green,
                       blue);

    sendCommand(payload, len);

    // Update local state
    state.red       = red;
    state.green     = green;
    state.blue      = blue;
    state.isRGBMode = true;
}

void WizBulb::setColorTemperature(int temperature) {
    if (udp_socket < 0) {
        return;
    }

    char payload[100];
    const int len = snprintf(payload, sizeof(payload), R"({"method":"setPilot","params":{"temp":%d}})", temperature);

    sendCommand(payload, len);

    // Update local state
    state.temperature = temperature;
    state.isRGBMode   = false;
}

void WizBulb::sendCommand(const char* payload, int len) {
    if (udp_socket < 0) {
        return;
    }

    sendto(udp_socket, payload, len, 0, reinterpret_cast<sockaddr*>(&wiz_addr), sizeof(wiz_addr));
}

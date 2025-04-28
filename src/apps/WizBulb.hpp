#ifndef WIZBULB_HPP
#define WIZBULB_HPP

#ifndef ESP_PLATFORM
#include "../platform/unix/Network.hpp"
#else
#include <WiFi.h>
#endif

#include <chrono>
#include <functional>
#include <string>
#include <vector>

// Define a struct for the bulb state
struct WizBulbState {
    bool isOnline   = false;
    bool isOn       = false;
    int brightness  = 50;
    int temperature = 2700;
    int red         = 255;
    int green       = 255;
    int blue        = 255;
    bool isRGBMode  = false;
};

class WizBulb {
public:
    WizBulb();
    ~WizBulb();

    void setBrightness(int brightness);
    void setColor(int red, int green, int blue);
    void setColorTemperature(int temperature);
    void setPower(bool state);

    // Callback type definition
    using StateCallback = std::function<void(const WizBulbState&)>;

    // Set callback for state updates
    void setStateCallback(StateCallback callback);

    // Request current state from the bulb
    void requestState();

    // Check if bulb is online
    bool isOnline() const;

    // Get current state
    const WizBulbState& getState() const;

    // Update function to be called in the main loop
    void update();

    // Set timeout duration in milliseconds
    void setTimeout(unsigned long timeoutMs);

private:
    WiFiUDP udp;
    IPAddress bulbIP;
    uint16_t bulbPort = 38899;  // WiZ light UDP port
    WizBulbState state;
    StateCallback stateCallback = nullptr;

    // Buffer for receiving data
    std::vector<char> receiveBuffer;

    // Timeout handling
    std::chrono::steady_clock::time_point lastRequestTime;
    unsigned long timeoutMs = 2000;
    bool timeoutOccurred    = false;
    bool pingInProgress     = false;

    void sendCommand(const char* payload, int len);

    // Process received state response
    void processStateResponse(const std::string& response);

    // Check for timeout
    void checkTimeout();
};

#endif  // WIZBULB_HPP

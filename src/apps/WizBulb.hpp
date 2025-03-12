//
// Created by Amitosh Swain Mahapatra on 2024.
//

#ifndef WIZBULB_HPP
#define WIZBULB_HPP

#ifdef ESP_PLATFORM
#include <lwip/sockets.h>
#else
#include <netinet/in.h>
#include <fcntl.h>
#endif

#include <functional>
#include <chrono>
#include <string>
#include <vector>

// Define a struct for the bulb state
struct WizBulbState {
    bool isOnline = false;
    bool isOn = false;
    int brightness = 50;
    int temperature = 2700;
    int red = 255;
    int green = 255;
    int blue = 255;
    bool isRGBMode = false;
};

class WizBulb {
public:
    WizBulb();
    ~WizBulb();

    void setBrightness(int brightness);
    void setColor(int red, int green, int blue);
    void setColorTemperature(int temperature);
    
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
    int udp_socket = -1;
    sockaddr_in wiz_addr = {};
    WizBulbState state;
    StateCallback stateCallback = nullptr;
    
    // Buffer for receiving data
    std::vector<char> receiveBuffer;
    
    // Timeout handling
    std::chrono::steady_clock::time_point lastResponseTime;
    unsigned long timeoutMs = 60000; // Default 60 seconds
    bool timeoutOccurred = false;
    
    void sendCommand(const char* payload, int len);
    
    // Process received state data
    void processStateResponse(const std::string& response);
    
    // Check for timeout
    void checkTimeout();
    
    // Make socket non-blocking
    void setNonBlocking(int sock);
};

#endif // WIZBULB_HPP

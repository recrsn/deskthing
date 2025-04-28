//
// Created by Amitosh Swain Mahapatra on 2024.
//

#include "WizBulb.hpp"

#include <ArduinoJson.h>

#include "platform/log.h"

static const char* LOG_TAG = "WizBulb";

WizBulb::WizBulb() {
    LOG_I(LOG_TAG, "Initializing WizBulb");
    udp.begin(0);

#ifdef BULB_IP
    // Parse IP address string
    int ip[4];
    sscanf(BULB_IP, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
    bulbIP = IPAddress(ip[0], ip[1], ip[2], ip[3]);
    LOG_I(LOG_TAG, "Bulb IP set to %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
#else
    LOG_W(LOG_TAG, "No bulb IP configured");
#endif

    // Initialize receive buffer
    receiveBuffer.resize(1024);
    lastRequestTime = std::chrono::steady_clock::now();
    timeoutOccurred  = false;
}

WizBulb::~WizBulb() {
    LOG_I(LOG_TAG, "Shutting down WizBulb");
    udp.stop();
}

void WizBulb::setStateCallback(StateCallback callback) {
    LOG_I(LOG_TAG, "Setting state callback");
    stateCallback = std::move(callback);
}

void WizBulb::requestState() {
    LOG_I(LOG_TAG, "Requesting bulb state");
    constexpr char payload[] = R"({"method":"getPilot"})";
    sendCommand(payload, strlen(payload));
}

bool WizBulb::isOnline() const {
    return state.isOnline;
}

const WizBulbState& WizBulb::getState() const {
    return state;
}

void WizBulb::setTimeout(unsigned long ms) {
    LOG_I(LOG_TAG, "Setting timeout to %lu ms", ms);
    timeoutMs = ms;
}

void WizBulb::checkTimeout() {
    if (!pingInProgress) {
        return;
    }

    const auto now     = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastRequestTime).count();

    // Only change state to offline if we weren't already timed out
    // This prevents flipping between states when multiple checks happen during timeout
    if (elapsed > timeoutMs) {
        // Mark bulb as offline
        state.isOnline  = false;
        timeoutOccurred = true;
        pingInProgress  = false;

        LOG_W(LOG_TAG, "Connection timed out after %lldms", elapsed);

        // Notify via callback
        if (stateCallback) {
            stateCallback(state);
        }
    }
}

void WizBulb::processStateResponse(const std::string& response) {
    // Mark the bulb as online since we received a response
    state.isOnline  = true;
    timeoutOccurred = false;
    pingInProgress  = false;

    // Update last response time
    lastRequestTime = std::chrono::steady_clock::now();

    // Parse the JSON response using ArduinoJson
    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, response);
    if (error) {
        // JSON parsing failed
        LOG_E(LOG_TAG, "JSON parsing failed: %s", error.c_str());
        if (stateCallback) {
            stateCallback(state);
        }
        return;
    }

    // Get the result object
    const JsonObject result = doc["result"];
    if (result.isNull()) {
        LOG_W(LOG_TAG, "No result object in response");
        if (stateCallback) {
            stateCallback(state);
        }
        return;
    }

    LOG_I(LOG_TAG, "Processing state response");

    // Get the state value
    if (result["state"].is<bool>()) {
        state.isOn = result["state"].as<bool>();
        LOG_D(LOG_TAG, "Bulb power state: %s", state.isOn ? "ON" : "OFF");
    }

    // Get the dimming (brightness) value
    if (result["dimming"].is<int>()) {
        state.brightness = result["dimming"].as<int>();
        LOG_D(LOG_TAG, "Brightness: %d", state.brightness);
    }

    // Check for RGB color values
    const bool hasRGB = result["r"].is<int>() && result["g"].is<int>() && result["b"].is<int>();
    if (hasRGB) {
        state.red       = result["r"].as<int>();
        state.green     = result["g"].as<int>();
        state.blue      = result["b"].as<int>();
        state.isRGBMode = true;
        LOG_D(LOG_TAG, "RGB mode: R=%d, G=%d, B=%d", state.red, state.green, state.blue);
    }

    // Check for color temperature
    if (result["temp"].is<int>()) {
        state.temperature = result["temp"].as<int>();
        state.isRGBMode   = false;
        LOG_D(LOG_TAG, "Temperature mode: %d K", state.temperature);
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
    int packetSize = udp.parsePacket();
    if (packetSize) {
        // Read the packet into our buffer
        int len = udp.read(receiveBuffer.data(), receiveBuffer.size() - 1);
        if (len > 0) {
            // Null-terminate the received data
            receiveBuffer[len] = '\0';

            LOG_D(LOG_TAG, "Received %d bytes: %s", len, receiveBuffer.data());

            // Process the response
            processStateResponse(std::string(receiveBuffer.data(), len));
        }
    }
}

void WizBulb::setBrightness(const int brightness) {
    LOG_I(LOG_TAG, "Setting brightness to %d", brightness);

    char payload[100];
    const int len = snprintf(payload, sizeof(payload), R"({"method":"setPilot","params":{"dimming":%d}})", brightness);

    sendCommand(payload, len);

    // Update local state
    state.brightness = brightness;
}

void WizBulb::setColor(const int red, const int green, const int blue) {
    LOG_I(LOG_TAG, "Setting color to RGB(%d, %d, %d)", red, green, blue);

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

void WizBulb::setColorTemperature(const int temperature) {
    LOG_I(LOG_TAG, "Setting color temperature to %d K", temperature);

    char payload[100];
    const int len = snprintf(payload, sizeof(payload), R"({"method":"setPilot","params":{"temp":%d}})", temperature);

    sendCommand(payload, len);

    // Update local state
    state.temperature = temperature;
    state.isRGBMode   = false;
}

void WizBulb::setPower(bool state) {
    LOG_I(LOG_TAG, "Setting power to %s", state ? "ON" : "OFF");

    char payload[100];
    const int len =
        snprintf(payload, sizeof(payload), R"({"method":"setPilot","params":{"state":%s}})", state ? "true" : "false");

    sendCommand(payload, len);

    // Update local state
    this->state.isOn = state;
}

void WizBulb::sendCommand(const char* payload, int len) {
#ifdef BULB_IP
    LOG_I(LOG_TAG, "Sending command: %s", payload);

    udp.beginPacket(bulbIP, bulbPort);
    udp.write(reinterpret_cast<const uint8_t*>(payload), len);
    udp.endPacket();

    pingInProgress   = true;
    lastRequestTime = std::chrono::steady_clock::now();
#else
    LOG_W(LOG_TAG, "Cannot send command: No bulb IP configured");
#endif
}

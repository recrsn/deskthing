#ifndef NETWORK_HPP
#define NETWORK_HPP

#ifdef ESP_PLATFORM
#include <WiFi.h>
#include <WiFiUdp.h>
#else
#include "unix/Network.hpp"
#endif
#endif  // NETWORK_HPP

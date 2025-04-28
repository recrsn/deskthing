#pragma once

#ifndef ESP_PLATFORM

#include <netinet/in.h>
#include <sys/socket.h>

#include <functional>

// IPAddress class to mimic Arduino's
class IPAddress {
    uint32_t address;

public:
    IPAddress();
    IPAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);
    explicit IPAddress(uint32_t address);
    explicit IPAddress(const uint8_t* address);

    bool operator==(const IPAddress& addr) const { return address == addr.address; }
    bool operator!=(const IPAddress& addr) const { return address != addr.address; }
    bool operator==(const uint32_t addr) const { return address == addr; }

    explicit operator uint32_t() const { return address; }

    uint8_t operator[](int index) const;
    uint8_t& operator[](int index);
};

// WiFiUDP class to mimic Arduino's
class WiFiUDP {
public:
    WiFiUDP();
    ~WiFiUDP();

    // Arduino WiFiUDP API
    uint8_t begin(uint16_t port);
    void stop();
    int beginPacket(IPAddress ip, uint16_t port) { return beginPacket((uint32_t)ip, port); }
    int beginPacket(uint32_t ip, uint16_t port);
    int endPacket();
    size_t write(uint8_t byte);
    size_t write(const uint8_t* buffer, size_t size);
    int parsePacket();
    int available();
    int read(unsigned char* buffer, size_t len);
    int read(char* buffer, size_t len);

private:
    int socket_fd;
    sockaddr_in remote_addr;
    socklen_t remote_addr_len;
    bool packet_available;
    unsigned char* receive_buffer;
    size_t buffer_size;
    size_t buffer_pos;
    size_t available_bytes;
};
#endif

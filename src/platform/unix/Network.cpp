#include "Network.hpp"

#if !defined(ESP_PLATFORM)
// On non-ESP32 platforms, we implement our own version of the WiFiUDP class
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>

#include <cstring>

// IPAddress implementation
IPAddress::IPAddress() : address(0) {}

IPAddress::IPAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet) {
    address = (first_octet << 24) | (second_octet << 16) | (third_octet << 8) | fourth_octet;
}

IPAddress::IPAddress(uint32_t addr) : address(addr) {}

IPAddress::IPAddress(const uint8_t* addr) {
    address = (addr[0] << 24) | (addr[1] << 16) | (addr[2] << 8) | addr[3];
}

uint8_t IPAddress::operator[](int index) const {
    return (address >> (8 * (3 - index))) & 0xFF;
}

uint8_t& IPAddress::operator[](int index) {
    static uint8_t dummy;
    if (index < 0 || index > 3) return dummy;
    
    uint8_t* bytes = (uint8_t*)&address;
    // Handle network byte order in a portable way
    return bytes[3 - index];
}

// WiFiUDP implementation
WiFiUDP::WiFiUDP() : socket_fd(-1), remote_addr_len(0), packet_available(false), 
                     receive_buffer(nullptr), buffer_size(0), buffer_pos(0), available_bytes(0) {
    memset(&remote_addr, 0, sizeof(remote_addr));
    // Allocate a reasonable buffer size for UDP packets
    buffer_size = 1500; // Standard MTU size
    receive_buffer = new unsigned char[buffer_size];
}

WiFiUDP::~WiFiUDP() {
    stop();
    if (receive_buffer) {
        delete[] receive_buffer;
        receive_buffer = nullptr;
    }
}

uint8_t WiFiUDP::begin(uint16_t port) {
    // Close existing socket if open
    if (socket_fd >= 0) {
        stop();
    }
    
    // Create UDP socket
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        perror("WiFiUDP::begin socket creation failed");
        return 0;
    }
    
    // Set socket to non-blocking mode
    int flags = fcntl(socket_fd, F_GETFL, 0);
    fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
    
    // Prepare server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    
    // Enable address reuse
    int opt = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // Bind socket
    if (bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("WiFiUDP::begin bind failed");
        close(socket_fd);
        socket_fd = -1;
        return 0;
    }
    
    return 1;
}

void WiFiUDP::stop() {
    if (socket_fd >= 0) {
        close(socket_fd);
        socket_fd = -1;
    }
    packet_available = false;
    buffer_pos = 0;
    available_bytes = 0;
}

int WiFiUDP::beginPacket(uint32_t ip, uint16_t port) {
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = htonl(ip);
    remote_addr.sin_port = htons(port);
    remote_addr_len = sizeof(struct sockaddr_in);
    
    return 1;
}

int WiFiUDP::endPacket() {
    if (socket_fd < 0) {
        return 0;
    }
    
    return 1;
}

size_t WiFiUDP::write(uint8_t byte) {
    return write(&byte, 1);
}

size_t WiFiUDP::write(const uint8_t* buffer, size_t size) {
    if (socket_fd < 0 || remote_addr_len == 0) {
        return 0;
    }
    
    ssize_t sent = sendto(socket_fd, buffer, size, 0, 
                          (struct sockaddr*)&remote_addr, sizeof(remote_addr));
    
    if (sent < 0) {
        perror("WiFiUDP::write sendto failed");
        return 0;
    }
    
    return sent;
}

int WiFiUDP::parsePacket() {
    if (socket_fd < 0) {
        return 0;
    }
    
    // Reset packet state
    packet_available = false;
    buffer_pos = 0;
    available_bytes = 0;
    
    // Try to receive data
    socklen_t addr_len = sizeof(remote_addr);
    ssize_t received = recvfrom(socket_fd, receive_buffer, buffer_size, 0,
                                (struct sockaddr*)&remote_addr, &addr_len);
    remote_addr_len = addr_len;
    
    if (received <= 0) {
        return 0;
    }
    
    packet_available = true;
    available_bytes = received;
    return received;
}

int WiFiUDP::available() {
    if (!packet_available) {
        return 0;
    }
    
    return available_bytes - buffer_pos;
}

int WiFiUDP::read(unsigned char* buffer, size_t len) {
    int avail = available();
    if (!avail) {
        return -1;
    }
    
    size_t to_read = (avail < len) ? avail : len;
    memcpy(buffer, &receive_buffer[buffer_pos], to_read);
    buffer_pos += to_read;
    
    return to_read;
}

int WiFiUDP::read(char* buffer, size_t len) {
    return read(reinterpret_cast<unsigned char*>(buffer), len);
}

#endif // End of non-ESP32 implementation
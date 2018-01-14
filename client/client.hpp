#pragma once

#include "ChaTTY_packets.h"
#include <array>
#include <string>
class ClientNet
{
    std::array<byte_t, 2048> buffer;
    static ClientNet* singleton;
    static void (*logMessage)(const char*);
    std::string username;
    void (*display_chat)(const char*, const char*);
    int socketfd;
    public:
    ClientNet(const char* hostnmae, const char* username, const unsigned long port);
    static ClientNet& get_singleton();
    static void hook_message_printing_fp(void (*fp)(const char*));
    void hook_display_chat(void (*fp)(const char*, const char*));
    void send_to_server(const char* message);
    void update();
};

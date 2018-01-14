#pragma once

class ClientNet
{
    static ClientNet* singleton;
    static void (*logMessage)(const char*);
    int socketfd;
    public:
    ClientNet(const char* hostnmae, const char* username, const unsigned long port);
    static ClientNet& get_singleton();
    static void hook_message_printing_fp(void (*fp)(const char*));
    void update();
};

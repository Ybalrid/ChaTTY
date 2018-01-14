#include "client.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include "ChaTTY_packets.h"

ClientNet* ClientNet::singleton = nullptr;
void (*ClientNet::logMessage)(const char*) = nullptr;

ClientNet::ClientNet(const char* host, const char* user, const unsigned long port) :
    socketfd{0},
    display_chat{nullptr},
    give_list{nullptr}
{
    if(singleton)
    {
        fprintf(stderr, "can't have multiple ClientNet instances!");
        exit(-1);
    }
    username = std::string(user);
    singleton = this;
    //check if port is valid
    if(!(port < 65536))
    {
        if(logMessage) {
            logMessage("Port out of normal range!");
            logMessage(std::to_string(port).c_str());
        }
        //handle error...
    }
    struct hostent* server = gethostbyname(host);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof serv_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);


    socketfd = socket(AF_INET, SOCK_STREAM, 0);
bool ok = true;
    if(connect(socketfd, reinterpret_cast<struct sockaddr*>(&serv_addr), sizeof serv_addr) <0)
    {
        ok = false;
        if(logMessage)
        {
            logMessage("Can't connect to server, connect failed");
            logMessage(host);
        }//handle error
    }

    if(ok)
    {
        ChaTTY_PACKET_(NAME_TRANSPORT) packet;
        memset(packet.names, '\0', strlen(user) + 2);
        strcpy(packet.names, user);
        packet.packetType = NAME_TRANSPORT;
        write(socketfd, ChaTTY_PACKET_SERIALIZE(&packet), sizeof packet);
    }
}

ClientNet& ClientNet::get_singleton()
{
    return *singleton;
}

void ClientNet::hook_message_printing_fp(void (*fp)(const char*))
{
    logMessage = fp;
}

void ClientNet::update()
{
    static int count;
    static int bytes_read;
    ioctl(socketfd, FIONREAD, &count);
    //if(logMessage) logMessage(std::to_string(count).c_str());
    if(count > 0)
    {
        bytes_read = read(socketfd, buffer.data(), count);

        switch(buffer[0])
        {
            case MESSAGE_TRANSPORT:
                {
                    ChaTTY_PACKET_(MESSAGE_TRANSPORT)* packet =
                        (ChaTTY_PACKET_(MESSAGE_TRANSPORT)*) buffer.data();
                    if(display_chat)
                    display_chat(packet->nickname, packet->message);

                }
                break;
            case NAME_TRANSPORT:
                {
                    ChaTTY_PACKET_(NAME_TRANSPORT) * packet =
                        (ChaTTY_PACKET_(NAME_TRANSPORT)*) buffer.data();
                    if(give_list)
                    give_list(packet->names);
                }
                break;
            default:
                logMessage("Unkonw packet type!");
        }
    }
}


void ClientNet::hook_display_chat(void (*fp)(const char*, const char*))
{
    display_chat = fp;
}

void ClientNet::send_to_server(const char* message)
{
    ChaTTY_PACKET_(MESSAGE_TRANSPORT) packet;
    packet.packetType = MESSAGE_TRANSPORT;
    strcpy(packet.message, message);
    strcpy(packet.nickname, username.c_str());
    write(socketfd, ChaTTY_PACKET_SERIALIZE(&packet), sizeof packet);
}

void ClientNet::hook_give_list(void (*fp)(const char*))
{
    give_list = fp;
}

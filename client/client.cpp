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
    socketfd{0}
{
    if(singleton)
    {
        fprintf(stderr, "can't have multiple ClientNet instances!");
        exit(-1);
    }

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
        strcpy(packet.name, user);
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

                }
                break;
            case NAME_TRANSPORT:
                break;
            default:
                logMessage("Unkonw packet type!");
        }
    }
}


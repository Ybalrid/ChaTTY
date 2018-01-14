#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "ChaTTY_packets.h"
#include "ChaTTY_common.h"

#include "client.hpp"
#include "ui.hpp"


void sanityCheck()
{
    ChaTTY_PACKET_(EMPTY) packet;
    packet.packetType = 42;

    printf("Packet type : %d\n", packet.packetType);

    byte_t* serialized = ChaTTY_PACKET_SERIALIZE(&packet);
    size_t byteLenght = sizeof packet;
    printf("First byte is : %d\n", serialized[0]);
    printf("lengh is : %d\n", byteLenght);
}

    auto append_text_function_pointer = [](const char* username, const char* message) {
        UserInterface::get_singleton().display_message(
                reinterpret_cast<const unsigned char*>(username),
                reinterpret_cast<const unsigned char*>(message));
    };
void loop(const char* message)
{
    append_text_function_pointer("myself", message);
}
int main(int argc, char* argv[])
{
    //set locale
    setlocale(LC_ALL, "");
    std::cout << "I'm in C++ \\o/\n";

    if(argc < 3)
    {
        puts("Please specify server address as first argument");
        puts("Please specify server port as second argument");
        return EXIT_FAILURE;
    }

    const char* hostname = argv[1];
    const unsigned long port = getPortFromStr(argv[2]);
    printf("Will connect to %s:%u\n", hostname, port);

    UserInterface ui;
    const auto username = ui.ask_for_username();
    ClientNet::hook_message_printing_fp([](const char* msg)
            {append_text_function_pointer("system", msg);});

    ClientNet network(hostname, username.c_str(), port);
    network.hook_display_chat(append_text_function_pointer);

    //Init network here
    ui.hook_send_messages([](const char* msg){
            ClientNet::get_singleton().send_to_server(msg);
            });
    //Give the UI a pointer to the function for sending messages to the server
    //Give the network code a function for giving received messages to the UI for display

    bool run = true;
    while(run)
    {
        ui.event_loop();

        //Do network transaction here
        network.update();
        if(ui.user_wants_to_quit() || false /*network lost*/)
        {
            run = false;
        }
    }


    return EXIT_SUCCESS;
}

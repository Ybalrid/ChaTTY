#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ChaTTY_packets.h"
#include "ChaTTY_common.h"

#include "ui.h"


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

int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        puts("Please specify server address as first argument");
        puts("Please specify server port as second argument");
        return EXIT_FAILURE;
    }

    const char* hostname = argv[1];
    const unsigned long port = getPortFromStr(argv[2]);
    printf("Will connect to %s:%u\n", hostname, port);

    init_ui();
    bool run = true;
    while(run)
    {
        event_loop();
        if(user_wants_to_quit() || false /*network lost*/)
        {
            run = false;
        }
    }


    return EXIT_SUCCESS;
}

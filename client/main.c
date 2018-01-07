#include "ChaTTY_packets.h"
#include <stdio.h>
#include <stdlib.h>

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
    if(argc < 2)
    {
        puts("Please specify server address as first argument");
        return EXIT_FAILURE;
    }

    const char* hostname = argv[1];
    printf("Will connect to %s\n", hostname);

    return EXIT_SUCCESS;
}

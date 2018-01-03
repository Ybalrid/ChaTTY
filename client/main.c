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
    sanityCheck();
    return EXIT_SUCCESS;
}

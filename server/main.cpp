#include <stdio.h>
#include <stdlib.h>
#include "ChaTTY_common.h"
#include "server.hpp"

int main(int argc, char* argv[])
{
    my_server(argc, argv);
    if(argc < 2)
    {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        return EXIT_FAILURE;
    }
    const unsigned long int port = getPortFromStr(argv[1]);
    printf("Will use port %u\n", port);



    return EXIT_SUCCESS;
}

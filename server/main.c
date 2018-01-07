#include <stdio.h>
#include <stdlib.h>
#include "ChaTTY_common.h"

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        puts("Please specify port to bind to as first argument");
        return EXIT_FAILURE;
    }
    const unsigned long int port = getPortFromStr(argv[1]);
    printf("Will use port %u\n", port);



    return EXIT_SUCCESS;
}

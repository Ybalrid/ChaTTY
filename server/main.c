#include <stdio.h>
#include <stdlib.h>
#include "ChaTTY_common.h"
#include "server.h"

int main(int argc, char* argv[])
{
    if (argc != 2) {
         fprintf(stderr, "Usage: %s port\n", argv[0]);
         exit(EXIT_FAILURE);
     }
    my_server_init(argv[1]);
    return EXIT_SUCCESS;
}

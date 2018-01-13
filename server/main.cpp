#include <stdio.h>
#include <stdlib.h>
#include "ChaTTY_common.h"
#include "server.hpp"

int                 main(int argc, char* argv[]) {
  s_my_server       my_srv;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s port\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  my_srv.service_str  = argv[1];
  my_srv.addr_str     = "0.0.0.0";
  my_srv.sfd          = -1;

  my_server_init(&my_srv);
  if (my_srv.sfd == -1) {
    return EXIT_FAILURE;
  }
  my_server_poll(&my_srv);
  return EXIT_SUCCESS;
}

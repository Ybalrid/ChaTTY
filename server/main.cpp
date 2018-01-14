#include <stdio.h>
#include <stdlib.h>
#include "ChaTTY_common.h"
#include "server.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

int                 main(int argc, char* argv[]) {
  s_my_server       my_srv;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s port\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  my_srv.service_str  = argv[1];
  my_srv.addr_str     = "0.0.0.0";
  my_srv.sfd          = -1;

  if (my_server_init(&my_srv) == -1) {
    fprintf(stderr, " > in my_server_init()\n");
    return EXIT_FAILURE;
  }
  my_server_poll(&my_srv);
  free(my_srv.events);
  close(my_srv.sfd);
  return EXIT_SUCCESS;
}

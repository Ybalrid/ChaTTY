#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include "ChaTTY_common.h"
#include "server.hpp"

in_port_t get_port(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return ntohs(((struct sockaddr_in*)sa)->sin_port);
  }
  else if (sa->sa_family == AF_INET6) {
    return ntohs(((struct sockaddr_in6*)sa)->sin6_port);
  }
  return (-1);
}

int my_server_init(char *param_service)
{
  struct addrinfo           hints;
  struct addrinfo           *result, *rp;
  char                      addr_str[INET6_ADDRSTRLEN];
  int                       sfd, s;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
  hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
  hints.ai_protocol = 0;          /* Any protocol */
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;
  /* Initialize the addrinfo hints struct for the listener */

  s = getaddrinfo(NULL, param_service, &hints, &result);
  if (s != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    exit(EXIT_FAILURE);
  }

  /* getaddrinfo() returns a list of address structures.
  Try each address until we successfully bind(2).
  If socket(2) (or bind(2)) fails, we (close the socket
  and) try the next address. */

  for (rp = result; rp != NULL; rp = rp->ai_next) {
    sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (sfd == -1) {
      continue;
    }

    if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
      /*inet_ntop(AF_INET, rp, addr_str, sizeof(addr_str)),
      get_port(rp->ai_addr));*/
      break;                  /* Success */
    }

    close(sfd);
  }

  if (rp == NULL) {               /* No address succeeded */
    fprintf(stderr, "Could not bind\n");
    exit(EXIT_FAILURE);
  }

  fprintf(stdout, "Listening at %s:%s\n", "0.0.0.0", param_service);
  /* Displays server state */

  freeaddrinfo(result);
}

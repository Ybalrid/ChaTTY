#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "ChaTTY_common.h"
#include "server.h"

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
  char                      host[NI_MAXHOST];
  char                      service[NI_MAXSERV];

  int                       sfd, s;
  struct sockaddr_storage   peer_addr;
  socklen_t                 peer_addr_len;
  ssize_t                   nread;
  char                      buf[BUF_SIZE];

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

  s = getnameinfo( rp,
    sizeof(*rp), host, NI_MAXHOST,
    service, NI_MAXSERV, NI_NUMERICSERV | NI_NUMERICHOST);
  if (s == 0) {
    fprintf(stdout, "Listening at %s:%d\n", host, service);
  }
  else {
    fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));
  }
  /* Displays server state */

  freeaddrinfo(result);

  /* Read datagrams and echo them back to sender */

  for (;;) {
    peer_addr_len = sizeof(struct sockaddr_storage);
    nread = recvfrom(sfd, buf, BUF_SIZE, 0,
      (struct sockaddr *) &peer_addr, &peer_addr_len);
      if (nread == -1)
      continue;               /* Ignore failed request */

      char host[NI_MAXHOST], service[NI_MAXSERV];

      s = getnameinfo((struct sockaddr *) &peer_addr,
      peer_addr_len, host, NI_MAXHOST,
      service, NI_MAXSERV, NI_NUMERICSERV);
      if (s == 0)
      printf("Received %ld bytes from %s:%s\n",
      (long) nread, host, service);
      else
      fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));

      if (sendto(sfd, buf, nread, 0,
        (struct sockaddr *) &peer_addr,
        peer_addr_len) != nread)
        fprintf(stderr, "Error sending response\n");
      }
    }

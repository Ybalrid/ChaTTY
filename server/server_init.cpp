#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/epoll.h>

#include "ChaTTY_common.h"

#include "server.hpp"


int                   my_server_init(s_my_server *my_srv) {
  if (my_server_init_network(my_srv) == -1) {
    fprintf(stderr, " > in my_server_init_network()\n");
    return (-1);
  }
  if (my_server_init_epoll(my_srv) == -1) {
    fprintf(stderr, " > in my_server_init_epoll()\n");
    return (-1);
  }
  return (0);
}

int                   my_server_init_network(s_my_server *my_srv) {
  int                 s;
  struct addrinfo     hints;
  struct addrinfo     *result, *rp;
  char                addr_str[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family     = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
  hints.ai_socktype   = SOCK_STREAM; /* Datagram socket */
  hints.ai_flags      = AI_PASSIVE;    /* For wildcard IP address */
  hints.ai_protocol   = 0;          /* Any protocol */
  hints.ai_canonname  = NULL;
  hints.ai_addr       = NULL;
  hints.ai_next       = NULL;
  /* Initialize the addrinfo hints struct for the listener */

  s = getaddrinfo(NULL, my_srv->service_str, &hints, &result);

  if (s != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    exit(EXIT_FAILURE);
  }

  /* getaddrinfo() returns a list of address structures.
  Try each address until we successfully bind(2).
  If socket(2) (or bind(2)) fails, we (close the socket
  and) try the next address. */

  for (rp = result; rp != NULL; rp = rp->ai_next) {
    my_srv->sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (my_srv->sfd == -1) {
      continue;
    }

    if (bind(my_srv->sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
      /*inet_ntop(AF_INET, rp, addr_str, sizeof(addr_str)),
      get_port(rp->ai_addr));*/
      break;                  /* Success */
    }

    close(my_srv->sfd);
  }

  if (rp == NULL) {               /* No address succeeded */
    fprintf(stderr, "Could not bind\n");
    exit(EXIT_FAILURE);
  }
  freeaddrinfo(result);

  if (my_server_unblock_socket(my_srv->sfd) == -1) {
    fprintf(stderr, " > in my_server_unblock_socket() : Could not make the socket in non-blocking mode\n");
    close(my_srv->sfd);
    return (-1);
  }

  if (listen(my_srv->sfd, MAX_CONNECTIONS) == -1) {
    /* For TCP only */
    /* inet_ntop(AF_INET, rp, addr_str, sizeof(addr_str)),
    get_port(rp->ai_addr)); */
    perror("listen()");
    close(my_srv->sfd);
    return (-1);
  }
  /* Sets the socket in listening state ready to get new clients */

  fprintf(stdout, "Listening at %s:%s\n", my_srv->addr_str, my_srv->service_str);
  /* Displays server state */
  return (0);
}

int                   my_server_init_epoll(s_my_server *my_srv) {
  struct epoll_event    event;
  int                   s;

  my_srv->efd = epoll_create1(0);
  if (my_srv->efd == -1) {
    perror("epoll_create1()");
    return (-1);
  }
  /* Creates the epoll instance */

  memset((void *)&event, 0, sizeof(event));
  event.data.fd = my_srv->sfd;
  event.events = EPOLLIN | EPOLLET;
  s = epoll_ctl(my_srv->efd, EPOLL_CTL_ADD, my_srv->sfd, &event);
  if (s == -1) {
    perror("epoll_ctl()");
    return (-1);
  }
  /* Adds the server socket to the event manager */

  my_srv->events = (epoll_event*)calloc(MAX_EVENTS, sizeof(*(my_srv->events)));
  /* Events storage init */

  return (0);
}


in_port_t             get_port(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return ntohs(((struct sockaddr_in*)sa)->sin_port);
  }
  else if (sa->sa_family == AF_INET6) {
    return ntohs(((struct sockaddr_in6*)sa)->sin6_port);
  }
  return (-1);
}

int                   my_server_unblock_socket(int fd) {
  int                 flags, s;

  flags = fcntl(fd, F_GETFL, 0);
  /* Gets current flags */
  if (flags == -1) {
    perror("fcntl(get)");
    return (-1);
  }

  flags |= O_NONBLOCK;
  /* Makes the socket non-blocking */

  s = fcntl(fd, F_SETFL, flags);
  /* Sets the edited flag to the fd */
  if (s == -1) {
    perror("fcntl(set)");
    return (-1);
  }
  return (0);
}

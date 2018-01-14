#include <netdb.h>
#include <sys/epoll.h>
#include <errno.h>
#include "ChaTTY_common.h"
#include "ChaTTY_packets.h"
#include "server.hpp"

int     my_server_poll(s_my_server *my_srv) {
  int   n;
  int   i;
  int   s;

  while (1) {
    n = epoll_wait(my_srv->efd, my_srv->events, MAX_EVENTS, -1);
    i = 0;

    while (i < n) {
      my_srv->curr_event_i = i; /* Updates struct info */

      if ((my_srv->events[i].events & EPOLLERR) ||
      (my_srv->events[i].events & EPOLLHUP) ||
      !(my_srv->events[i].events & EPOLLIN)) {
        /* Handle any error */
        my_server_e_error(my_srv);
      }
      else if (my_srv->events[i].data.fd == my_srv->sfd) {
        /* The server socket have incoming connection(s) */
        do {
          s = my_server_e_incoming_conn(my_srv);
        } while (s == 0);
        if (s == -1) {
          fprintf(stderr, " > in my_server_e_incoming_conn()\n");
        }
      }
      else {
        /* Incoming data from a client */
        if (my_server_e_incoming_data(my_srv) == -1) {
          fprintf(stderr, " > in my_server_e_incoming_data()\n");
        }
      }
      i++;
    }
  }
  return (0);
}

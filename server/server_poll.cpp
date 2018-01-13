#include <netdb.h>
#include <sys/epoll.h>
#include "ChaTTY_common.h"
#include "ChaTTY_packets.h"
#include "server.hpp"

int     my_server_poll(s_my_server *my_srv) {
  int   n;
  int   i;

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
        continue;
      }
      else if (my_srv->events[i].data.fd == my_srv->sfd) {
        /* The server socket have incoming connection(s) */
        my_server_e_incoming_conn(my_srv);
      }
      i++;
    }
  }
  return (0);
}

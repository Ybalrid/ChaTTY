#include <list>

#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#include "ChaTTY_common.h"
#include "ChaTTY_packets.h"
#include "server.hpp"

int                         my_server_e_incoming_conn(s_my_server *my_srv) {
  int                       s, cfd;
  struct epoll_event        event;
  s_my_client               *client;

  client = (s_my_client *)malloc(sizeof(*client));
  if (client == NULL) {
    perror("malloc()");
    return (-1);
  }

  client->peer_addr_len = sizeof(struct sockaddr_storage);
  /* Avoid multiple calculation */
  client->cfd = accept(my_srv->sfd, (struct sockaddr *) &client->peer_addr, &client->peer_addr_len);
  /* TCP only: waits until a new connection is made */
  if (cfd < 0) {
    perror("accept()");
    free(client);
    return (-1);
  }

  s = my_server_unblock_socket(client->cfd);
  if (s == -1) {
    fprintf(stderr, " > in my_server_unblock_socket()\n");
    free(client);
    return (-1);
  }

  event.data.ptr = client;

  //event->data.fd = client->cfd;
  printf("fd:%d\n", event.data.ptr);
  fflush(stdout);
  event.events = EPOLLIN | EPOLLET;
  s = epoll_ctl(my_srv->efd, EPOLL_CTL_ADD, client->cfd, &event);
  if (s == -1) {
    perror("epoll_ctl()");
    free(client);
    return (-1);
  }
  /* Adds the new client to the epoll */

  my_srv->channel.clients.push_back(client);
  /* Adds the fd to the channel */

  s = getnameinfo((struct sockaddr *) &client->peer_addr, client->peer_addr_len,
   client->addr_str, NI_MAXHOST, client->service_str, NI_MAXSERV, NI_NUMERICSERV);
  if (s == 0) {
    printf("Incoming connection from %s:%s\n", client->addr_str, client->service_str);
  }
  else {
    fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));
  }
  /* Displays the state */

  return (0);
}

int                         my_server_e_incoming_data(s_my_server *my_srv) {
  ssize_t                   nread;
  char                      buf[BUF_SIZE];
  struct epoll_event        *event;
  s_my_client               *client;

  event = &my_srv->events[my_srv->curr_event_i];
  client = (s_my_client *)event->data.ptr;
  nread = recv(client->cfd, buf, BUF_SIZE, 0 /* | MSG_DONTWAIT*/ );
  /* TCP form to receive data */
  /* UDP : nread = recvfrom(cfd, buf, BUF_SIZE, 0, (struct sockaddr *) &peer_addr, &peer_addr_len); */

  if (nread == -1) {
    return (-1);               /* Ignore failed request */
  }

  printf("Received %ld bytes from %s:%s\n", (long) nread, client->addr_str, client->service_str);


  for (auto it: my_srv->channel.clients) {
      printf("Broadcast to %s:%s\n", it->addr_str, it->service_str);
      if (send(it->cfd, buf, nread, 0) != nread) {
        /* UDP : sendto(cfd, buf, nread, 0, (struct sockaddr *) &peer_addr, peer_addr_len) != nread */
        fprintf(stderr, "Error sending response\n");
      }
  }

  return (0);
}

int                         my_server_e_error(s_my_server *my_srv) {
  struct epoll_event        *event;
  s_my_client               *client;

  event = &my_srv->events[my_srv->curr_event_i];
  client = (s_my_client *)event->data.ptr;

  fprintf(stderr, "Event error occurred on fd %d. Closing.\n", client->cfd);
  close(client->cfd);
  free(client);
  //my_srv->channel.clients.insert(cfd);
}

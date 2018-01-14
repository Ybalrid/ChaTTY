#include <list>

#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ChaTTY_common.h"
#include "ChaTTY_packets.h"
#include "server.hpp"

int                         my_server_e_incoming_conn(s_my_server *my_srv) {
  int                       s;
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
  if (client->cfd < 0) {
    free(client);

    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      /* No more available incoming connection */
      return (1);
    }
    perror("accept()");
    return (-1);
  }

  s = my_server_unblock_socket(client->cfd);
  if (s == -1) {
    fprintf(stderr, " > in my_server_unblock_socket()\n");
    free(client);
    return (-1);
  }

  event.data.ptr = client;
  /* DO NOT FILL data.fd or it will overwrite data.ptr ! */

  event.events = EPOLLIN | EPOLLET;
  s = epoll_ctl(my_srv->efd, EPOLL_CTL_ADD, client->cfd, &event);
  if (s == -1) {
    perror("epoll_ctl()");
    free(client);
    return (-1);
  }
  /* Adds the new client to the epoll */

  s = getnameinfo((struct sockaddr *) &client->peer_addr, client->peer_addr_len,
   client->addr_str, NI_MAXHOST, client->service_str, NI_MAXSERV, NI_NUMERICSERV);
  if (s == 0) {
    printf("Incoming connection %d from %s:%s\n", client->cfd, client->addr_str, client->service_str);
  }
  else {
    fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));
  }
  /* Displays the state */

  my_server_send_motd(my_srv, client);
  my_server_add_client(my_srv, client);

  return (0);
}

int                         my_server_e_incoming_data(s_my_server *my_srv) {
  ssize_t                   nread;
  char                      buf[BUF_SIZE];
  struct epoll_event        *event;
  s_my_client               *client;

  event = &my_srv->events[my_srv->curr_event_i];
  client = (s_my_client *)event->data.ptr;

  while ((nread = recv(client->cfd, buf, BUF_SIZE, 0 /* | MSG_DONTWAIT*/ )) > 0) {
    /* Get all the buffer */

    /* TCP form to receive data */
    /* UDP : nread = recvfrom(cfd, buf, BUF_SIZE, 0, (struct sockaddr *) &peer_addr, &peer_addr_len); */
    printf("Received %ld bytes from %s:%s\n", (long)nread, client->addr_str, client->service_str);

    for (auto it: my_srv->channel.clients) {
        printf("Broadcast to %s:%s\n", it->addr_str, it->service_str);
        if (send(it->cfd, buf, nread, 0) != nread) {
          /* UDP : sendto(cfd, buf, nread, 0, (struct sockaddr *) &peer_addr, peer_addr_len) != nread */
          fprintf(stderr, "Error sending response\n");
        }
    }
    /* Broadcast for everybody in the channel */
  }

  if (nread == -1 && errno != EAGAIN) {
      perror("read");
      return (-1);
  }
  /* Filter true errors */

  if (nread == 0) {
    fprintf(stdout, "Client %d has just exited.\n", client->cfd);
    my_server_e_close(my_srv);
  }
  /* End of "file" */


  return (0);
}

int                         my_server_e_error(s_my_server *my_srv) {
  struct epoll_event        *event;
  s_my_client               *client;

  event = &my_srv->events[my_srv->curr_event_i];
  client = (s_my_client *)event->data.ptr;

  fprintf(stderr, "Event error occurred on fd %d. Closing.\n", client->cfd);
  my_server_e_close(my_srv);
  return (0);
}

int                         my_server_e_close(s_my_server *my_srv) {
  struct epoll_event        *event;
  s_my_client               *client;

  event = &my_srv->events[my_srv->curr_event_i];
  client = (s_my_client *)event->data.ptr;

  close(client->cfd);
  my_server_remove_client(my_srv);
  free(client);
  return (0);
}

int                         my_server_add_client(s_my_server *my_srv, s_my_client *client) {
  ssize_t                   nread;
  char                      buf[BUF_SIZE];

  my_srv->channel.clients.push_back(client);
  /* Adds the client into the channel */

  nread = snprintf(buf, BUF_SIZE, "Client %d has just joined.\n", client->cfd);
  for (auto it: my_srv->channel.clients) {
      printf("Broadcast to %s:%s\n", it->addr_str, it->service_str);
      if (send(it->cfd, buf, nread, 0) != nread) {
        /* UDP : sendto(cfd, buf, nread, 0, (struct sockaddr *) &peer_addr, peer_addr_len) != nread */
        fprintf(stderr, "Error sending message\n");
      }
  }
  /* Broadcast for everybody in the channel */


  return (0);
}


int                         my_server_remove_client(s_my_server *my_srv) {
  ssize_t                   nread;
  char                      buf[BUF_SIZE];
  struct epoll_event        *event;
  s_my_client               *client;

  event = &my_srv->events[my_srv->curr_event_i];
  client = (s_my_client *)event->data.ptr;

  my_srv->channel.clients.remove(client);

  nread = snprintf(buf, BUF_SIZE, "Client %d has just left.\n", client->cfd);
  for (auto it: my_srv->channel.clients) {
      printf("Broadcast to %s:%s\n", it->addr_str, it->service_str);
      if (send(it->cfd, buf, nread, 0) != nread) {
        /* UDP : sendto(cfd, buf, nread, 0, (struct sockaddr *) &peer_addr, peer_addr_len) != nread */
        fprintf(stderr, "Error sending message\n");
      }
  }
  /* Broadcast for everybody in the channel */


  return (0);
}

int                         my_server_send_motd(s_my_server *my_srv, s_my_client *client) {
  ssize_t                   nread;
  char                      buf[BUF_SIZE];
  int                       motd_fd;

  motd_fd = open(MOTD_FILE, O_RDONLY);
  if (motd_fd < 0) {
    perror("open motd file");
    return (-1);
  }
  nread = snprintf(buf, BUF_SIZE, "Votre addresse de connexion est %s:%s\n", client->addr_str, client->service_str);
  if (send(client->cfd, buf, nread, 0) != nread) {
    fprintf(stderr, "Error sending MOTD\n");
    return (-1);
  }
  while ((nread = read(motd_fd, buf, BUF_SIZE)) > 0) {
    if (send(client->cfd, buf, nread, 0) != nread) {
      fprintf(stderr, "Error sending MOTD\n");
      return (-1);
    }
  }
  if (nread < 0) {
    perror("read motd file");
    return (-1);
  }

  close(motd_fd);

  return (0);
}

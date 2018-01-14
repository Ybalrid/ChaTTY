#include <list>
#include <algorithm>

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

  //my_server_send_motd(my_srv, client);

  return (0);
}

int                         my_server_e_incoming_data(s_my_server *my_srv) {
  struct epoll_event        *event;
  s_my_client               *client;
  ssize_t                   nread;
  byte_t                    buf[ChaTTY_PACKET_MAX_SIZE];
  int                       s;

  event = &my_srv->events[my_srv->curr_event_i];
  client = (s_my_client *)event->data.ptr;

  nread = recv(client->cfd, buf, ChaTTY_PACKET_MAX_SIZE, MSG_DONTWAIT);
  /* Get data type */
  /* UDP : nread = recvfrom(cfd, buf, BUF_SIZE, 0, (struct sockaddr *) &peer_addr, &peer_addr_len); */

  s = 0;
  if (nread > 0) {
    switch (buf[0]) {

      case (MESSAGE_TRANSPORT):
        if (nread == ChaTTY_PACKET_SIZE(MESSAGE_TRANSPORT)) {
            strncpy(((ChaTTY_PACKET_(MESSAGE_TRANSPORT)*)&buf)->nickname, client->nickname, sizeof(((ChaTTY_PACKET_(MESSAGE_TRANSPORT)*)&buf)->nickname));
            my_server_sendto_channel(&my_srv->channel, buf, ChaTTY_PACKET_SIZE(MESSAGE_TRANSPORT));
        }
        else {
          s = 1;
        }
      break;

      case (NAME_TRANSPORT):
        if (nread == ChaTTY_PACKET_SIZE(NAME_TRANSPORT)) {
          strncpy(client->nickname, ((ChaTTY_PACKET_(NAME_TRANSPORT)*)&buf)->names, sizeof(client->nickname));
          if (!(std::find(my_srv->channel.clients.begin(), my_srv->channel.clients.end(), client) != my_srv->channel.clients.end())) {
            my_server_channel_join(&my_srv->channel, client);
            my_server_send_motd(my_srv, client);
          }
          else {
            // TODO Notif updated nickname
          }
          my_server_send_clients_list(&my_srv->channel);
        }
        else {
          s = 1;
        }
      break;

      default:
        fprintf(stderr, "Unknown type %u received from %s:%s\n", buf[0], client->addr_str, client->service_str);
        my_server_e_close(my_srv);
        return (-1);
    }

    fprintf(stdout, "Received %u bytes of type %u from %s:%s\n", nread, buf[0], client->addr_str, client->service_str);
  }


  if (s == 1) {
    fprintf(stderr, "Invalid data size %u for type %u received from %s:%s\n", nread, buf[0], client->addr_str, client->service_str);
    my_server_e_close(my_srv);
    return (-1);
  }

  if (nread == -1 && errno != EAGAIN) {
    perror("recv");
    my_server_e_close(my_srv);
    return (-1);
  }
  /* Filter true errors */

  if (nread == -1 && errno == EAGAIN) {
    return (1);
  }
  /* No more data in buffer */

  if (nread == 0) {
    fprintf(stdout, "Client %d has just exited.\n", client->cfd);
    my_server_e_close(my_srv);
    return (1);
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
  my_server_channel_quit(&my_srv->channel, client);
  free(client);
  return (0);
}

int                         my_server_channel_join(s_my_channel *channel, s_my_client *client) {
  int                       motd_fd;
  ChaTTY_PACKET_(MESSAGE_TRANSPORT) data;

  channel->clients.push_back(client);
  /* Adds the client into the channel */

  memset((void *)&data, 0, sizeof(data));
  data.packetType = (MESSAGE_TRANSPORT);
  strncpy(data.nickname, SERVER_NAME, sizeof(data.nickname));
  snprintf(data.message, sizeof(data.message), "%s vient de nous rejoindre.\n", client->nickname);
  my_server_sendto_channel(channel, (byte_t *)&data, sizeof(data));
  return (0);
}


int                         my_server_channel_quit(s_my_channel *channel, s_my_client *client) {
  int                       motd_fd;
  ChaTTY_PACKET_(MESSAGE_TRANSPORT) data;

  channel->clients.remove(client);
  /* Removes the client from the channel */

  memset((void *)&data, 0, sizeof(data));
  data.packetType = (MESSAGE_TRANSPORT);
  strncpy(data.nickname, SERVER_NAME, sizeof(data.nickname));
  snprintf(data.message, sizeof(data.message), "%s vient de nous quitter pour un autre monde...\n", client->nickname);

  my_server_send_clients_list(channel);
  my_server_sendto_channel(channel, (byte_t *)&data, sizeof(data));

  return (0);
}

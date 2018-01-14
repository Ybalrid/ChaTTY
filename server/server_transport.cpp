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

int                         my_server_sendto_channel(s_my_channel *channel, byte_t *data, ssize_t len) {

  for (auto client: channel->clients) {
      my_server_sendto_client(client, data, len);
  }
  /* Broadcast for everybody in the channel */
  return (0);
}

int                         my_server_sendto_client(s_my_client *client, byte_t *data, ssize_t len) {

  fprintf(stdout, "Sending %u bytes of type %u to %s:%s\n", (long)len, (byte_t)data[0], client->addr_str, client->service_str);
  if (send(client->cfd, data, len, 0) != len) {
    /* UDP : sendto(cfd, buf, nread, 0, (struct sockaddr *) &peer_addr, peer_addr_len) != nread */
    fprintf(stderr, "Error sending message\n");
    return (-1);
  }
  return (0);
}

int                         my_server_send_motd(s_my_server *my_srv, s_my_client *client) {
  ssize_t                   nread;
  int                       motd_fd;
  ChaTTY_PACKET_(MESSAGE_TRANSPORT) msg;

  msg.packetType = (MESSAGE_TRANSPORT);
  strncpy(msg.nickname, SERVER_NAME, sizeof(msg.nickname));
  nread = snprintf(msg.message, sizeof(msg.message), "Votre adresse de connexion est %s:%s\n", client->addr_str, client->service_str);
  if (my_server_sendto_client(client, (byte_t *)&msg, sizeof(msg))) {
    fprintf(stderr, "Error sending MOTD\n");
    return (-1);
  }

  motd_fd = open(MOTD_FILE, O_RDONLY);
  if (motd_fd < 0) {
    perror("open motd file");
    return (-1);
  }

  while ((nread = read(motd_fd, msg.message, sizeof(msg.message))) > 0) {
    if (my_server_sendto_client(client, (byte_t *)&msg, sizeof(msg))) {
      fprintf(stderr, "Error sending MOTD\n");
      close(motd_fd);
      return (-1);
    }
  }

  if (nread < 0) {
    perror("read motd file");
    close(motd_fd);
    return (-1);
  }

  close(motd_fd);

  return (0);
}

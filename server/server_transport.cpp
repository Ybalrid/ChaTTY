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

int                         my_server_sendto_channel(s_my_server *my_srv, s_my_channel *channel, byte_t *data, ssize_t len) {

  for (auto it: channel->clients) {
      fprintf(stdout, "Sending to %s:%s\n", it->addr_str, it->service_str);
      if (send(it->cfd, data, len, 0) != len) {
        /* UDP : sendto(cfd, buf, nread, 0, (struct sockaddr *) &peer_addr, peer_addr_len) != nread */
        fprintf(stderr, "Error sending message\n");
      }
  }
  /* Broadcast for everybody in the channel */
  return (0);
}

int                         my_server_send_motd(s_my_server *my_srv, s_my_client *client) {
  ssize_t                   nread;
  char                      buf[ChaTTY_PACKET_MAX_SIZE];
  int                       motd_fd;

  nread = snprintf(buf, sizeof(buf), "Votre addresse de connexion est %s:%s\n", client->addr_str, client->service_str);
  if (send(client->cfd, buf, nread, 0) != nread) {
    fprintf(stderr, "Error sending MOTD\n");
    return (-1);
  }

  motd_fd = open(MOTD_FILE, O_RDONLY);
  if (motd_fd < 0) {
    perror("open motd file");
    return (-1);
  }

  while ((nread = read(motd_fd, buf, sizeof(buf))) > 0) {
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

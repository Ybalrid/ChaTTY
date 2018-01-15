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

int                       my_server_send_clients_list(s_my_channel *channel) {
  ChaTTY_PACKET_(NAME_TRANSPORT) data;
  unsigned int i;

  memset((void *)&data, 0, sizeof(data));
  data.packetType = (NAME_TRANSPORT);
  i = 0;
  for (auto client: channel->clients) {
      if (i + strlen(client->nickname) + 2 > sizeof(data.names)) {
        /* Too many clients */
        fprintf(stderr, "Too many client in the channel! \n");
        break ;
      }
      strncpy(data.names + i, client->nickname, sizeof(data.names) - i);
      i += strlen(client->nickname) + 1;
  }
  data.names[i + 1] = '\0';
  my_server_sendto_channel(channel, (byte_t*) &data, sizeof(data));
}

int                         my_server_send_motd(s_my_server *my_srv, s_my_client *client) {
  ssize_t                   nread;
  FILE                      *motd_file;
  char                      *motd_buf;
  size_t                   motd_buf_len;
  ChaTTY_PACKET_(MESSAGE_TRANSPORT) data;

  memset((void *)&data, 0, sizeof(data));
  data.packetType = (MESSAGE_TRANSPORT);
  strncpy(data.nickname, SERVER_NAME, sizeof(data.nickname));
  nread = snprintf(data.message, sizeof(data.message), "Votre adresse de connexion est %s:%s\n", client->addr_str, client->service_str);
  if (my_server_sendto_client(client, (byte_t *)&data, sizeof(data))) {
    fprintf(stderr, "Error sending MOTD\n");
    return (-1);
  }

  motd_file = fopen(MOTD_FILE, "r");
  if (motd_file == NULL) {
    perror("open motd file");
    return (-1);
  }

  motd_buf = NULL;
  motd_buf_len = 0;
  while ((nread = getline(&motd_buf, &motd_buf_len, motd_file)) > 0) {
    strncpy(data.message, motd_buf, sizeof(data.message));

    if (my_server_sendto_client(client, (byte_t *)&data, sizeof(data))) {
      fprintf(stderr, "Error sending MOTD\n");
      fclose(motd_file);
      return (-1);
    }
  }

  if (nread < 0) {
    perror("read motd file");
    fclose(motd_file);
    return (-1);
  }

  fclose(motd_file);

  return (0);
}

#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include "ChaTTY_common.h"
#include "ChaTTY_packets.h"
#include "server.hpp"

int                         my_server_e_incoming_conn(s_my_server *my_srv) {
  int                       s, cfd;
  struct sockaddr_storage   peer_addr;
  socklen_t                 peer_addr_len;
  ssize_t                   nread;
  char                      buf[BUF_SIZE];
  char                      host[NI_MAXHOST], service[NI_MAXSERV];



  peer_addr_len = sizeof(struct sockaddr_storage);
  /* Avoid multiple calculation */
  cfd = accept(my_srv->sfd, (struct sockaddr *) &peer_addr, &peer_addr_len);
  /* TCP only: waits until a new connection is made */

  return (0);
}

int                         my_server_e_incoming_data(s_my_server *my_srv) {
  int                       s, cfd;
  struct sockaddr_storage   peer_addr;
  socklen_t                 peer_addr_len;
  ssize_t                   nread;
  char                      buf[BUF_SIZE];
  char                      host[NI_MAXHOST], service[NI_MAXSERV];

  nread = recv(cfd, buf, BUF_SIZE, 0 /* | MSG_DONTWAIT*/ );
  /* TCP form to receive data */
  /* UDP : nread = recvfrom(cfd, buf, BUF_SIZE, 0, (struct sockaddr *) &peer_addr, &peer_addr_len); */

  if (nread == -1) {
    return (-1);               /* Ignore failed request */
  }

  s = getnameinfo((struct sockaddr *) &peer_addr, peer_addr_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);
  if (s == 0) {
    printf("Received %ld bytes from %s:%s\n", (long) nread, host, service);
  }
  else {
    fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));
  }

  if (send(cfd, buf, nread, 0) != nread) {
    /* UDP : sendto(cfd, buf, nread, 0, (struct sockaddr *) &peer_addr, peer_addr_len) != nread */
    fprintf(stderr, "Error sending response\n");
  }
  return (0);
}

int                         my_server_e_error(s_my_server *my_srv) {
  fprintf(stderr, "Event error occurred on fd %d. Closing.\n", my_srv->events[my_srv->curr_event_i].data.fd);
  close(my_srv->events[my_srv->curr_event_i].data.fd);
}

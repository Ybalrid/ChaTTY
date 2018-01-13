#include <netdb.h>
#include <sys/epoll.h>
#include "ChaTTY_common.h"
#include "ChaTTY_packets.h"
#include "server.h"

int my_server_poll() {
  /* Read datagrams and echo them back to sender */

  int                       sfd, s;
  struct sockaddr_storage   peer_addr;
  socklen_t                 peer_addr_len;
  ssize_t                   nread;
  char                      buf[BUF_SIZE];
  struct                    epoll_event ev, *events;
  char                      host[NI_MAXHOST], service[NI_MAXSERV];


  for (;;) {
    peer_addr_len = sizeof(struct sockaddr_storage);
    nread = recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr *) &peer_addr, &peer_addr_len);
    if (nread == -1) {
      continue;               /* Ignore failed request */
    }

    s = getnameinfo((struct sockaddr *) &peer_addr, peer_addr_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);
    if (s == 0) {
      printf("Received %ld bytes from %s:%s\n", (long) nread, host, service);
    }
    else {
      fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));
    }

    if (sendto(sfd, buf, nread, 0, (struct sockaddr *) &peer_addr, peer_addr_len) != nread) {
      fprintf(stderr, "Error sending response\n");
    }
  }
}

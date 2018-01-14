#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include <netdb.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <list>
#define MAX_CONNECTIONS   SOMAXCONN
#define MAX_EVENTS        42
#define MOTD_FILE         "./motd.txt"

typedef struct {
  int                       cfd;
  struct sockaddr_storage   peer_addr;
  socklen_t                 peer_addr_len;
  char                      service_str[NI_MAXSERV];
  char                      addr_str[NI_MAXHOST];
  char                      *nickname;
}                           s_my_client;

typedef struct {
  int             id;
  char            *name;
  std::list<s_my_client *>  clients;
}                 s_my_channel;

typedef struct {
  int                 sfd;            /* server fd */
  int                 efd;            /* epoll event fd */
  struct epoll_event  *events;        /* epoll events list */
  int                 curr_event_i;   /* current epoll event index */
  char                *service_str;   /* port */
  char                *addr_str;      /* host */
  s_my_channel        channel;       /* unique channel */
}                     s_my_server;

int       my_server_init(s_my_server *);
int       my_server_init_network(s_my_server *);
int       my_server_init_epoll(s_my_server *);
int       my_server_unblock_socket(int);
int       my_server_poll(s_my_server *);
int       my_server_e_incoming_conn(s_my_server *);
int       my_server_e_incoming_data(s_my_server *);
int       my_server_e_error(s_my_server *);
int       my_server_send_motd(s_my_server *, s_my_client *);


#endif

#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include <sys/socket.h>
#include <sys/epoll.h>
#define MAX_CONNECTIONS   SOMAXCONN
#define MAX_EVENTS        42

typedef struct {
  int                 sfd;            /* server fd */
  int                 efd;            /* epoll event fd */
  struct epoll_event  *events;        /* epoll events list */
  int                 curr_event_i;   /* current epoll event index */
  char                *service_str;   /* port */
  char                *addr_str;      /* host */
} s_my_server;

typedef struct {
  int     cfd;
  char    *service_str;
  char    *addr_str;
} s_my_client;

int       my_server_init(s_my_server *);
int       my_server_init_network(s_my_server *);
int       my_server_init_epoll(s_my_server *);
int       my_server_unblock_socket(int);
int       my_server_poll(s_my_server *);
int       my_server_e_incoming_conn(s_my_server *);
int       my_server_e_incoming_data(s_my_server *);
int       my_server_e_error(s_my_server *);


#endif

#ifndef _SERVER_HPP_
#define _SERVER_HPP_

/* #include <sys/socket.h>
#define MAX_CONNECTIONS SOMAXCONN */
#define MAX_CONNECTIONS 42

typedef struct {
  int     sfd;
  char    *service_str;
  char    *addr_str;
} s_my_server;

typedef struct {
  int     cfd;
  char    *service_str;
  char    *addr_str;
} s_my_client;

int       my_server_init(s_my_server *);
int       my_server_unblock_socket(int);
int       my_server_poll(s_my_server *);

#endif

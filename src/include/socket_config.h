#ifndef SOCKS_CONFIG_H
#define SOCKS_CONFIG_H

#include <inttypes.h>


/* not sure if I want to use it */
#define TCPKEEPALIVE 1
#define TCPKEEPALIVE_KEEPCNT 10
#define TCPKEEPALIVE_KEEPIDLE 1
#define TCPKEEPALIVE_KEEPINTVL 1

void socket_enable_keepalive(int sock);

/* gets ipv4 address and port form socket file descriptor */
/* NOTE: buffer and bufsz must be at least INET6_ADDRSTRLEN or bigger */
int socket_peer_addrinfo(int sock, char *buffer, size_t bufsz, uint16_t *port);

#endif
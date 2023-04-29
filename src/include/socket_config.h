#ifndef SOCKS_CONFIG_H
#define SOCKS_CONFIG_H

#include <inttypes.h>

/* not sure if I want to use it */
void socket_enable_keepalive(int sock);

/* gets ipv4 address and port form socket file descriptor */
/* NOTE: buffer and bufsz must be at least INET6_ADDRSTRLEN or bigger */
int socket_peer_addrinfo(int sock, char *buffer, size_t bufsz, uint16_t *port);

#endif
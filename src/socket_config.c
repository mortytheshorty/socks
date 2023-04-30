#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/signal.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "include/socks_internal.h"

#define check(expr) if (!(expr)) { perror(#expr); return -1; }

int socket_enable_keepalive(int sock) {
#if 0
    int yes = 1;
    check(setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int)) != -1);

    int idle = 1;
    check(setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int)) != -1);

    int interval = 1;
    check(setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int)) != -1);

    int maxpkt = 10;
    check(setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int)) != -1);
#endif
    // thanks to user Haris from stackoverflow for this solution to reducde duplication
    struct option {
        int level;
        int opt_name;
        const void *opt_val;
    }

    const options[] = {
        {SOL_SOCKET, SO_KEEPALIVE, (int[]) {TCPKEEPALIVE}},
        {IPPROTO_TCP, TCP_KEEPCNT, (int[]) {TCPKEEPALIVE_KEEPCNT}},
        {IPPROTO_TCP, TCP_KEEPIDLE, (int[]) {TCPKEEPALIVE_KEEPIDLE}},
        {IPPROTO_TCP, TCP_KEEPINTVL, (int[]) {TCPKEEPALIVE_KEEPINTVL}},
    };

    for(size_t i = 0; i < ARRAYSIZE(options); i++) {
        check(setsockopt(sock, options[i].level, options[i].opt_name, options[i].opt_val, sizeof(options->opt_val)) != -1);
    }

    return 0;
}


void *get_in_addr(const struct sockaddr_storage *storage) {

    if( storage->ss_family == AF_INET) // IPv4 address
        return &(((struct sockaddr_in*)storage)->sin_addr);
    // else IPv6 address
    return &(((struct sockaddr_in6*)storage)->sin6_addr);
}

int socket_peer_addrinfo(int sock, char *buffer, size_t bufsz, uint16_t *port)
{
    if(bufsz != INET6_ADDRSTRLEN) {
        return -1;
    }

    struct sockaddr_storage inaddr = { 0 };
    socklen_t inaddr_len = sizeof(inaddr);
    
    if(getpeername(sock, (struct sockaddr*) &inaddr, &inaddr_len) != 0) {
        perror("socket_peer_addrinfo: getpeername()");
    }

    if(inet_ntop(inaddr.ss_family, get_in_addr(&inaddr), buffer, inaddr_len) == NULL) {
        return -1;
    }
    
    if(port) {
        *port = ntohs(((struct sockaddr_in*)&inaddr)->sin_port);
    }

    return 0;
}

int socket_local_addrinfo(int sock, char *buffer, size_t bufsz, uint16_t *port)
{
    if(bufsz < INET6_ADDRSTRLEN) {
        return -1;
    }

    struct sockaddr_storage inaddr = { 0 };
    socklen_t inaddr_len = sizeof(inaddr);
    
    getsockname(sock, (struct sockaddr*) &inaddr, &inaddr_len);

    if(inet_ntop(inaddr.ss_family, get_in_addr(&inaddr), buffer, inaddr_len) == NULL) {
        return -1;
    }
    
    if(port) {
        *port = ntohs(((struct sockaddr_in*)&inaddr)->sin_port);
    }

    return 0;
}

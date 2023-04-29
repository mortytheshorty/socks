#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/signal.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define check(expr) if (!(expr)) { perror(#expr); kill(0, SIGTERM); }

void socket_enable_keepalive(int sock) {
    int yes = 1;
    check(setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int)) != -1);

    int idle = 1;
    check(setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int)) != -1);

    int interval = 1;
    check(setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int)) != -1);

    int maxpkt = 10;
    check(setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int)) != -1);
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

    struct sockaddr_storage inaddr;
    socklen_t inaddr_len = sizeof(inaddr);
    
    getpeername(sock, (struct sockaddr*) &inaddr, &inaddr_len);

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

    struct sockaddr_storage inaddr;
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

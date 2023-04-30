#ifndef SOCKS_INTERNAL_H
#define SOCKS_INTERNAL_H

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <pthread.h>

#include "../../include/socks.h"
#include "socket_config.h"
#include "socks4.h"

#define ARRAYSIZE(x) (sizeof((x)) / sizeof((x)[0]))



ssize_t readFromSocket(int sock, void *buf, size_t size);
ssize_t sendToSocket(int sock, void *buf, size_t size);

void *socks_connection_thread(void *pipefdsp);
int socks_accept(socks *proxy, socks_connection *conn);
int socks_log(socks *socks, const char *function, const char *fmt, ...);
#define socks_log(socks, fmtstr, ...) socks_log(socks, __func__, fmtstr, ##__VA_ARGS__)

#endif
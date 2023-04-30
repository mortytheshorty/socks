#include <stdio.h>
#include <pthread.h>

#include "include/socks_internal.h"


int socks_listen(socks *proxy)
{
    if(proxy->config->type != SOCKS_SERVER) {
        socks_log(proxy, "can not listen to socks handle. socks handle is a client");
        return -1;
    }

    int conidx = 0;

    if(listen(proxy->sock, proxy->config->maxconn) != 0) {
        socks_log(proxy, "failed to listen on %s", proxy->config->ip);
        return SOCKS_LISTEN_ERR; // SOCKS_LISTEN_ERROR see: errno
    }

    socks_log(proxy, "listening on '%s:%d'", proxy->config->ip, proxy->config->port);

    for(;;) {
        pthread_t thread;
        socks_connection conn = { 0 };
        int ret = socks_accept(proxy, &conn);
        if(ret < 0) {
            // SOCKS_ACCEPT ERROR
            socks_log(proxy, "failed to accept connection from %s", "unkown");
            continue;
        }

        pthread_create(&thread, NULL, socks_connection_thread, &conn);
        conidx++;
        
    }

    return 0;
}
//
// Created by flo on 4/18/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "../../include/socks.h"

socks proxy;

int main(void)
{


    socks_config config;
    config.type = SOCKS_SERVER;
    config.version = SOCKS4_VERSION;
    config.ip = "127.0.0.1";
    config.port = (uint16_t) 1080;
    config.filename = "server.log";
    config.maxconn = 1024;

    int ret;

    if((ret = socks_init(&proxy, &config)) != 0) {
        printf("socks_server_init: %d\n", ret);
        exit(EXIT_FAILURE);
    }

    ret = socks_listen(&proxy);
    if(ret) {
        printf("socks_listen: %d\n", ret);
        exit(EXIT_FAILURE);
    }

    socks_destroy(&proxy);


}
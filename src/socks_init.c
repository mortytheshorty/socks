//
// Created by flo on 4/17/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/ioctl.h>

#include "include/socks_internal.h"


int socks_init(socks *socks_handle, socks_config *config)
{
    int rc = 0;
    int on = 1;

    socks_handle->config = config;

    if(config->filename)
    {
        socks_handle->log = fopen(config->filename, "w");

        if( !socks_handle->log ) {
            fprintf(stderr, "error: can't open logfile '%s'\n", config->filename);
            return SOCKS_LOGCREAT_ERR;
        }

    }

    socks_log(socks_handle, "Socks%d-%s started.", config->version, (config->type == SOCKS_SERVER) ? "Server" : "Client");

    // initialize socks main socket
    socks_handle->sock = socket(AF_INET, SOCK_STREAM, 0);
    if(socks_handle->sock < 0)
    {
        socks_log(socks_handle, "failed to create socket");
        return SOCKS_SOCKCREAT_ERR; // SOCKS_SOCKET_CREATION_ERROR
    }

    /* this is SOCKS VERSION dependent */
    struct sockaddr_in inaddr;
    memset(&inaddr, 0, sizeof(inaddr));

    inaddr.sin_family = AF_INET;
    inaddr.sin_port = htons((uint16_t) config->port);
    inaddr.sin_addr.s_addr = inet_addr(config->ip);
    if(inaddr.sin_addr.s_addr == INADDR_NONE)
    {
        socks_log(socks_handle, "failed to parse ip address %s", config->ip);
        return SOCKS_IP_INVAL; // SOCKS_WRONG_IP_ADDRESS
    }

    /* if server, bind address to socket and set it reusable */
    if(config->type == SOCKS_SERVER)
    {
        rc = setsockopt(socks_handle->sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        if(rc < 0) {
            socks_log(socks_handle, "failed to enable reuse of socket address");
            return SOCKS_SOCKREUSE_ERR;
        }

        if( bind(socks_handle->sock, (struct sockaddr*) &inaddr, sizeof(inaddr)) != 0 )
        {
            socks_log(socks_handle, "failed to bind %s to socket.", config->ip);
            return SOCKS_SERVER_BIND_ERR; // SOCKS_BIND_ERROR
        }

    }

    /* if client, try to connect to proxy server */
    else if(config->type == SOCKS_CLIENT)
    {
        if(connect(socks_handle->sock, (struct sockaddr*)&inaddr, sizeof(inaddr)) != 0)
        {
            socks_log(socks_handle, "failed to connect to socks server %s", config->ip);
            return SOCKS_CLIENT_CONNECT_ERR; // SOCKS_CLIENT_CONNECT_ERROR
        }
        socks_log(socks_handle, "connected to socker server %s:%d", config->ip, config->port);
    }

    return 0;
}

/* sends data via proxy server */
ssize_t socks_send(socks *client, void *data, size_t len)
{
    return sendToSocket(client->sock, data, len);
}

/* reads data from proxy server */
ssize_t socks_recv(socks *client, void *data, size_t len)
{   
    return readFromSocket(client->sock, data, len);
}

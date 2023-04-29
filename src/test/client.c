//
// Created by flo on 4/18/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "../../include/socks.h"
#define PROXY_ADDRESS "127.0.0.1"
#define PROXY_PORT 1080
#define ECHO_SERVER 1081 // run with: ncat -e /bin/cat -k -l 1081

int main(void)
{
    socks_config config;

    config.ip = PROXY_ADDRESS;
    config.port = (uint16_t) PROXY_PORT;
    config.type = SOCKS_CLIENT;
    config.version = SOCKS4_VERSION;
    config.filename = "client.log";
    

    socks client;
    int ret;

    ret = socks_init(&client, &config);
    if(ret) {
        printf("socks_client: %d\n", ret);
        exit(EXIT_FAILURE);
    }
    
    ret = socks_connect(&client, "127.0.0.1", ECHO_SERVER);

    if(ret) {
        printf("socks_client_connect: %d\n", ret);
        exit(EXIT_FAILURE);
    }

    char msg[] = "Echo Test Message";
    char buffer[sizeof(msg)];

    socks_send(&client, msg, sizeof(msg));
    socks_recv(&client, buffer, sizeof(buffer));

    printf("Send....: %s\n", msg);
    printf("Received: %s\n", buffer);


    socks_destroy(&client);

    return 0;
}
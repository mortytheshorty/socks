//
// Created by flo on 4/18/23.
//

#ifndef SOCKS_SOCKS_H
#define SOCKS_SOCKS_H

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdarg.h>


#define SOCKS4_VERSION 0x04

/* Socks types*/
enum SOCKS_TYPE {SOCKS_SERVER, SOCKS_CLIENT};


typedef struct socks_config {
    int type; // SOCKS_SERVER or SOCKS_CLIENT
    int maxconn; // maximum connections possible if SOCKS_SERVER else irgnored
    char *ip; // SOCKS_SERVER == interface the to listen on, SOCKS_CLIENT == server address to connect to
    uint16_t port; // SOCKS_SERVER == port to listen on, SOCKS_CLIENT == socks server port to connect to
    char *filename; // log file name
    uint8_t version; // socks version
} socks_config;

typedef struct socks {
    socks_config *config; // pointer to socks config
    int sock; // network socket
    char *ip; // destination ip address to connect to via socks proxy or unused as server
    uint16_t port; // destination port to connect to via socks proxy or unused as server
    FILE *log; // log file handle
} socks;

typedef struct socks_connection {
    int client_sock;
    int target_sock;
} socks_connection;

/**
 * @brief Initializes the socks structure for client and server aswell
 * 
 * @param socks socks strucuture
 * @param config socks configruation
 * @retval 0 on success
 * @retval n on failure
 */
int socks_init(socks *socks, socks_config *config);

/**
 * @brief Connectes a socks client with a remote peer via the proxy
 * 
 * @param client socks client structure
 * @param ip destination ip
 * @param port destination port
 * @retval 0 on success
 * @retval n on failure
 */
int socks_connect(socks *client, char *ip, int port);

/**
 * @brief Starts listening on configured server
 * 
 * @param proxy socks server structure
 * @retval 0 on success
 * @retval n on failure
 */
int socks_listen(socks *proxy);


/**
 * @brief Sends data of specified size to socks socket
 * 
 * @param socks socks socks structure
 * @param data data
 * @param len size of data
 * @retval n on success
 * @retval -1 on failure
 */
ssize_t socks_send(socks *socks, void *data, size_t len);

/**
 * @brief Receives data of specified size to socks socket
 * 
 * @param socks socks socks structure
 * @param data data
 * @param len size of data
 * @retval n on success
 * @retval -1 on failure
 */
ssize_t socks_recv(socks *socks, void *data, size_t len);

/**
 * @brief Destroys socks structure
 * 
 * @param socks socks strucutre
 */
void socks_destroy(socks *socks);



#endif //SOCKS_SOCKS_H

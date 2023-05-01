#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>

#define _GNU_SOURCE
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <poll.h>

#include "include/socks_internal.h"

#define MAX_SOCKETS 2

#define DEFAULT_TIMEOUT (3 * 60 * 1000)

#define CLIENT_POLL 0
#define REMOTE_POLL 1

int timeout = DEFAULT_TIMEOUT;

void *socks_connection_thread(void *pipefd) {

    pthread_detach(pthread_self());
    socks_connection conn = *(socks_connection*) pipefd;
    int rc = 0;

    int timeout = DEFAULT_TIMEOUT;
    /* two pollfd's for listening on remote peer and client connection for events */
    struct pollfd pfds[MAX_SOCKETS] = { 0 };
    nfds_t nfds = MAX_SOCKETS;

    uint8_t client_buf[4096] = { 0 };
    size_t client_buf_size = 0;

    uint8_t target_buf[4096] = { 0 };
    size_t target_buf_size = 0;
    
    ssize_t num_bytes = 0;


    /* set sockets to non blocking */
    int flags = fcntl(conn.client_sock, F_GETFL, 0);
    if(fcntl(conn.client_sock, F_SETFL, flags | O_NONBLOCK) != 0) {
        perror("socks_connection_thread: fcntl(conn.client_sock, ...)");
    }

    flags = fcntl(conn.target_sock, F_GETFL, 0);
    if(fcntl(conn.target_sock, F_SETFL, flags | O_NONBLOCK) != 0) {
        perror("socks_connection_thread: fcntl(conn.target_sock, ...)");
    }



    while(true) {
        

        pfds[CLIENT_POLL].fd = conn.client_sock;
        pfds[CLIENT_POLL].events = POLLIN;

        pfds[REMOTE_POLL].fd = conn.target_sock;
        pfds[REMOTE_POLL].events = POLLIN;

        /* waiting for some events */
        rc = poll(pfds, nfds, timeout);
        if(rc < 0) {
            fprintf(stderr, "poll() failed: %s\n", strerror(errno));
            break;
        }

        if(rc == 0) {
            fprintf(stderr, "poll() timed out. End Connection\n");
            break;
        }
        

        /* there is something to read form the client side */
        if(pfds[CLIENT_POLL].revents & POLLIN)
        {
            //printf("Got data from client.\n");
            num_bytes = readFromSocket(conn.client_sock, &client_buf[client_buf_size], sizeof(client_buf)-client_buf_size);
            if(num_bytes < 0) break; // client connection lost
            if(num_bytes > 0) {
                printf("read from client: %s (%ld)\n", client_buf, num_bytes);
                client_buf_size += (size_t) num_bytes;
                pfds[REMOTE_POLL].revents = POLLOUT;
            }
        }
        /* there is something to read from the remote side */
        else if(pfds[REMOTE_POLL].revents & POLLIN)
        {
            //printf("Got data from remote.\n");
            num_bytes = readFromSocket(conn.target_sock, target_buf, sizeof(target_buf));
            if (num_bytes < 0) break; // remote connection lost
            if (num_bytes > 0) {
                printf("read from peer: %s (%ld)\n", target_buf, (size_t)  num_bytes);
                target_buf_size += (size_t) num_bytes;
                pfds[CLIENT_POLL].revents = POLLOUT;
            }
        }

        /* client has data to receive */
        if(pfds[CLIENT_POLL].revents & POLLOUT) {
            //printf("Got data from client.\n");
            num_bytes = sendToSocket(conn.client_sock, target_buf, target_buf_size);
            if (num_bytes < 0) break;
            if (num_bytes > 0) {
                printf("forward to client: %s (%ld)\n", target_buf, num_bytes);
                // remove the sent bytes...
                target_buf_size -= (size_t) num_bytes;
                memmove(client_buf, &client_buf[num_bytes], client_buf_size);

            } 
        } else if(pfds[REMOTE_POLL].revents & POLLOUT) {
            //printf("Got data from remote.\n");
            num_bytes = sendToSocket(conn.target_sock, client_buf, (size_t) num_bytes);
            if(num_bytes < 0) break;
            if(num_bytes > 0) {
                printf("forward to remote peer: %s (%ld)\n", client_buf, num_bytes);
                // remove the sent bytes...
                client_buf_size -= (size_t) num_bytes;
                memmove(target_buf, &target_buf[num_bytes], target_buf_size);
            }

        }

        else {
            // unexpected event result appeared so close the connection
            break;
        }

    }

    // all done
    if(close(conn.client_sock) != 0) {
        perror("socks_connection_thread: close(conn.client_sock):");
    } 
    if(close(conn.target_sock) != 0) {
        perror("socks_connection_thread: close(conn.target_sock):");
    }
    printf("Thread terminating\n");
    return NULL;
}

#include "include/socks_internal.h"



int target_connect(socks *proxy, socks4_request *req, int client_sock, int *target_sock)
{
    char ip[INET6_ADDRSTRLEN];
    uint16_t port;
    socks4_reply reply = { 0 };
    struct sockaddr_in inaddr = { 0 };
    

    inaddr.sin_family = AF_INET;
    inaddr.sin_addr.s_addr = req->ip;
    inaddr.sin_port = req->port;

    *target_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(*target_sock < 0) {
        socks_log(proxy, "failed to create remote peer socket.");
        return SOCKS_SOCKCREAT_ERR; // SOCKS_SERVER_TARGET_SOCKET_CREATE_ERROR
    }

    socket_peer_addrinfo(client_sock, ip, sizeof(ip), &port);
    // we try to connect to the remote peer for the client
    if(connect(*target_sock, (struct sockaddr*) &inaddr, sizeof(inaddr)) != 0) {
        reply.version = SOCKS4_VERSION;
        reply.code = SOCKS4_REPLY_FAILED;
        reply.ip = inaddr.sin_addr.s_addr;
        reply.port = inaddr.sin_port;
        
        if(sendToSocket(client_sock, &reply, sizeof(reply)) != sizeof(reply)) {
            socks_log(proxy, "failed to send reply about failed connect to %s:%d", ip, port);
            return SOCKS_SEND_ERR;
        }

        socks_log(proxy, "failed to connect to remote peer '%s:%d'", ip, port);

        return SOCKS_SERVER_CONNECT_ERR; // SOCKS_SERVER_TARGET_CONNECT_ERROR
    }

    socks_log(proxy, "CONNECT from %s:%d to %s:%d", ip, port, inet_ntoa(*(struct in_addr*) &req->ip), ntohs(req->port));
    return SOCKS_OK;
}

int socks_accept(socks *proxy, socks_connection *conn)
{
    if(proxy->config->type != SOCKS_SERVER) {
        socks_log(proxy, "can not listen to socks handle. socks handle is a client");
    }
    
    socks4_request request = { 0 };
    socks4_reply reply = { 0 };

    char ip[INET6_ADDRSTRLEN] = { 0 };
    uint16_t port;
    int target_sock = -1;
    int client_sock = -1;
    int rc = 0;
    
    client_sock = accept(proxy->sock, NULL, NULL);
    if(client_sock < 0) {
        socks_log(proxy, "socks_accept: failed to accept connection from %s", "unknown");
        return SOCKS_SERVER_ACCEPT_ERR; // SOCKS_SERVER_ACCEPT_ERROR see: errno
    }

    socket_peer_addrinfo(client_sock, ip, sizeof(ip), &port);
    socks_log(proxy, "connection from %s:%d", ip, port);

    if(readFromSocket(client_sock, &request, sizeof(request)) != sizeof(request)) {
        socks_log(proxy, "failed to receive request from '%s:%d'", ip, port);
        if(close(client_sock) != 0) {
            perror("socks_accept: close(client_sock)");
        }
        return SOCKS_RECV_ERR; // SOCKS_SERVER_RECV_ERROR 
    }
    
    if(request.userid != 0) {
        // identifaction protocol code
    }

    rc = target_connect(proxy, &request, client_sock, &target_sock);
    if( rc != SOCKS_OK ) {
        return SOCKS_SERVER_CONNECT_ERR;
    }
    
    // build the reply
    reply.version = SOCKS4_VERSION;
    reply.code = SOCKS4_REPLY_GRANTED;
    reply.port = request.port;
    reply.ip = request.ip;

    // send it back to the client
    if(sendToSocket(client_sock, &reply, sizeof(reply)) != sizeof(reply)) {
        socks_log(proxy, "failed to send reply about successful connect to '%s:%d'", "unknown", 0);
        if(close(client_sock) != 0) {
            perror("socks_accept: close(client_sock)");
        }
        return SOCKS_SEND_ERR; // SOCKS_SERVER_SEND_ERROR
    }

    conn->client_sock = client_sock;
    conn->target_sock = target_sock;

    return 0;
}

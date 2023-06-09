#include "include/socks_internal.h"

int socks_connect(socks *client, char *ip, int port) {
    
    client->ip = ip;
    client->port = (uint16_t) port;

    ssize_t ret = 0;
    socks4_request request = { 0 };

    request.version = SOCKS4_VERSION;
    request.cmd = SOCKS4_CMD_CONNECT;
    request.port = htons((uint16_t) port);
    request.ip = inet_addr(ip);
    request.userid[0] = 0x00;

    if((ret = sendToSocket(client->sock, &request, sizeof(request))) != sizeof(request)) {
        socks_log(client, "failed to send CONNECT request");
        return SOCKS_SEND_ERR;
    }
    
    socks4_reply reply = { 0 };

    if((ret = readFromSocket(client->sock, &reply, sizeof(reply))) != sizeof(reply)) {
        socks_log(client, "failed to receive reply from socks server '%s:%d'", client->config->ip, client->config->port);
        return SOCKS_RECV_ERR;
    }

    switch(reply.code) {
        case SOCKS4_REPLY_GRANTED:
            socks_log(client, "connection to '%s:%d' established", client->ip, client->port);
            return 0;

        case SOCKS4_REPLY_FAILED:
            socks_log(client, "connection to '%s:%d' denied", client->ip, client->port);
            return SOCKS_REQUEST_DENY;

        default:
            socks_log(client, "received unsupported reply code '%d' (X'%x')", reply.code, reply.code);
            return SOCKS_REPLY_INVAL;
    }

}
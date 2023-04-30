//
// Created by flo on 4/18/23.
//

#ifndef SOCKS_SOCKS4_H
#define SOCKS_SOCKS4_H

#include <inttypes.h>

#if defined(__GNUC__) || defined(__clang__)
 #define PACKED __attribute__((__packed__))
#else
 #define PACKED
#endif


#define SOCKS4_VERSION 0x04
#define SOCKS4_CMD_CONNECT 0x01
#define SOCKS4_CMD_BIND 0x02

struct PACKED socks4_request {
    uint8_t version;
    uint8_t cmd;
    uint16_t port; // network byte order
    uint32_t ip; // network byte order
    uint8_t userid[1];
};
typedef struct socks4_request socks4_request;


#define SOCKS4_REPLY_GRANTED 0x5A
#define SOCKS4_REPLY_FAILED 0x5B
// #define SOCKS4_REPLY_IDENT_CONNECT_FAILED 0x5C
// #define SOCKS4_REPLY_IDENT_AUTH_FAILED 0x5D

struct PACKED socks4_reply {
    uint8_t version;
    uint8_t code;
    uint16_t port;
    uint32_t ip;
};
typedef struct socks4_reply socks4_reply;


enum SOCKS4_ERRORS {
    SOCKS_OK,
    SOCKS_SEND_ERR,
    SOCKS_RECV_ERR,
    SOCKS_REPLY_INVAL,
    SOCKS_REQUEST_DENY,
    SOCKS_SOCKCREAT_ERR,
    SOCKS_LOGCREAT_ERR,
    SOCKS_IP_INVAL,
    SOCKS_SOCKREUSE_ERR,
    SOCKS_SERVER_BIND_ERR,
    SOCKS_CLIENT_CONNECT_ERR,
    SOCKS_SERVER_ACCEPT_ERR,
    SOCKS_SERVER_CONNECT_ERR, // server can not connect to destination
    SOCKS_LISTEN_ERR,
};

#endif //SOCKS_SOCKS4_H

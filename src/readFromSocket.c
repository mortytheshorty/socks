#include "include/socks_internal.h"

ssize_t readFromSocket(int sock, void *buf, size_t size)
{
    char *pbuf = buf;
    ssize_t num_bytes = 0, total = 0;
    
    while (total < size) {
        num_bytes = recv(sock, &pbuf[total], size-total, 0);
        if (num_bytes < 0) {
            if ((num_bytes < 0) && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
                break;
            }
            return -1;
        }
        total +=  num_bytes;
    }

    return total;
}


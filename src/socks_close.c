#include "include/socks_internal.h"

void socks_destroy(socks *socks_handle)
{
    close(socks_handle->sock);
    socks_log(socks_handle, __FUNCTION__, "closing connection");
    if(socks_handle->log) {
        fclose(socks_handle->log);
    }
}
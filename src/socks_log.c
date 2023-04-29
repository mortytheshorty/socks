#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <stdarg.h>
#include <errno.h>

#include "include/socks_internal.h"

int socks_log(socks *socks, const char *function, const char *fmt, ...)
{
    
    if(socks->log) {
        va_list args;
        va_start(args, fmt);
        time_t now = time(NULL); // get current time
        char buffer[128];
        struct tm tnow;

        gmtime_r(&now, &tnow); // create UTC time

        strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S-%Z", &tnow); // create UTC time format string

        fprintf(socks->log, "%s: ", buffer); // log timestamp
        vfprintf(socks->log, fmt, args); // log message

        if(errno) { // if errno is set, log calling function with error string
            fprintf(socks->log, "%s: %s", function, strerror(errno));
        }
        va_end(args);

        fputc('\n', socks->log);
        fflush(socks->log);
    }

    return 0;
}

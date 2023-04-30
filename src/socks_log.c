#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <stdarg.h>
#include <errno.h>

#include "include/socks4.h"
#include "include/socket_config.h"
#include "../include/socks.h"

#define TIME_BUF_LEN (11 /* year */ + 5*2 /* mdHMS */ + 6 /* separators */ )

int socks_log(socks *socks, const char *function, const char *fmt, ...)
{
    
    if(socks->log) {
        va_list args;
        va_start(args, fmt);
        char buffer[TIME_BUF_LEN * 2 + 1];

        time_t now = time(NULL); // get current time
        if (now == -1) {
            strcpy(buffer, "Unknown time");
        }
        else {
            struct tm *tm_now = gmtime(&now);
            if (tm_now == NULL) {
                strcpy(buffer, "Unknown time");
            }
            else {
                if (strftime(buffer, sizeof buffer, "%Y-%m-%dT%H:%M:%SZ", tm_now) == 0) {
                    strcpy(buffer, "Unknown time");
                }
            }
        }
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

#include "log.h"
#include <stdio.h>
#include <time.h>

#define LOG_FILE "app.log"

static void write_log(const char *type, const char *message)
{
    FILE *fp = fopen(LOG_FILE, "a");
    if (!fp)
        return;

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[64];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(fp, "[%s] %s: %s\n", time_buf, type, message);
    fclose(fp);
}

void log_request(const char *message)
{
    write_log("REQUEST", message);
}

void log_response(const char *message)
{
    write_log("RESPONSE", message);
}

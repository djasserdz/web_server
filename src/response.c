#include "response.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

void send_response(int client_fd, struct response *res, const char *body, long body_size, const char *content_type)
{
    char buffer[1024];

    char date_str[128];
    if (res->Date && res->Date->value)
    {
        strncpy(date_str, res->Date->value, sizeof(date_str));
    }
    else
    {
        time_t now = time(NULL);
        struct tm tm_now;
        gmtime_r(&now, &tm_now);
        strftime(date_str, sizeof(date_str), "%a, %d %b %Y %H:%M:%S GMT", &tm_now);
    }

    const char *server_str = (res->Server && res->Server->value) ? res->Server->value : "MyCServer/0.1";

    long n = snprintf(buffer, sizeof(buffer),
                      "%s %d %s\r\n"
                      "Server: %s\r\n"
                      "Date: %s\r\n"
                      "Content-Length: %ld\r\n"
                      "Content-Type: %s\r\n"
                      "\r\n",
                      res->http_version, res->status_code, res->message,
                      server_str, date_str,
                      body_size, content_type);

    send(client_fd, buffer, n, 0);

    if (body_size > 0 && body)
    {
        send(client_fd, body, body_size, 0);
    }
}

void set_response(struct response *res, int status_code)
{
    res->status_code = status_code;
    strcpy(res->http_version, "HTTP/1.1");

    switch (status_code)
    {
    case 200:
        strcpy(res->message, "OK");
        break;
    case 404:
        strcpy(res->message, "Not Found");
        break;
    case 500:
        strcpy(res->message, "Internal Server Error");
        break;
    case 401:
        strcpy(res->message, "Unauthorized");
        break;
    case 403:
        strcpy(res->message, "Forbidden");
        break;
    case 201:
        strcpy(res->message, "Created");
        break;
    default:
        strcpy(res->message, "Unknown");
        break;
    }
}

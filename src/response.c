#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "response.h"

void send_response(int client_fd, struct response *res, const char *body)
{
    char buffer[1024];

    int body_length = body ? strlen(body) : 0;
    int n = snprintf(buffer, sizeof(buffer),
                     "%s %d %s\r\n"
                     "Content-Length: %d\r\n"
                     "Content-Type: text/html\r\n"
                     "\r\n",
                     res->http_version, res->status_code, res->message,
                     body_length);

    send(client_fd, buffer, n, 0);

    if (body_length > 0)
    {
        send(client_fd, body, body_length, 0);
    }
}

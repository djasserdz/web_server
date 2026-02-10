#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "request.h"
#include "log.h"

char *methods[9] = {"GET", "POST", "DELETE", "HEAD", "OPTIONS", "PATCH", "TRACE", "CONNECT"};
char *versions[3] = {"HTTP/1.0", "HTTP/1.1", "HTTP/2.0"};

int is_valid_method(const char *method)
{
    for (int i = 0; i < 8; i++)
        if (strcmp(method, methods[i]) == 0)
            return 1;
    return 0;
}

int is_valid_version(const char *version)
{
    for (int i = 0; i < 3; i++)
        if (strcmp(version, versions[i]) == 0)
            return 1;
    return 0;
}

void parse_request(char *buffer, ssize_t length, struct request *req)
{
    char *line_end = strstr(buffer, "\r\n");
    if (!line_end)
    {
        perror("malformed request");
        return;
    }
    int line_length = line_end - buffer;
    char request_line[512];
    strncpy(request_line, buffer, line_length);
    request_line[line_length] = '\0';

    sscanf(request_line, "%s %s %s", req->method, req->path, req->http_version);

    if (!is_valid_method(req->method))
    {
        req->invalid_method = 1;
        return;
    }

    if (!is_valid_version(req->http_version))
    {
        req->invalid_version = 1;
        return;
    }

    char *header_start = line_end + 2;
    while (header_start < buffer + length)
    {
        char *header_end = strstr(header_start, "\r\n");
        if (!header_end || header_end == header_start)
            break;

        char line[512];
        int line_len = header_end - header_start;
        strncpy(line, header_start, line_len);
        line[line_len] = '\0';

        char *colon = strstr(line, ": ");
        if (colon)
        {
            *colon = '\0';
            char *key = line;
            char *value = colon + 2;

            if (strcasecmp(key, "Host") == 0)
            {
                req->Host = malloc(sizeof(Header));
                req->Host->key = strdup(key);
                req->Host->value = strdup(value);
            }
            else if (strcasecmp(key, "Content-Type") == 0)
            {
                req->Content_type = malloc(sizeof(Header));
                req->Content_type->key = strdup(key);
                req->Content_type->value = strdup(value);
            }
            else if (strcasecmp(key, "Content-Length") == 0)
            {
                req->Content_length = malloc(sizeof(Header));
                req->Content_length->key = strdup(key);
                req->Content_length->value = strdup(value);
            }
        }

        header_start = header_end + 2;
    }

    char log_msg[512];
    snprintf(log_msg, sizeof(log_msg), "%s %s %s", req->method, req->path, req->http_version);
    log_request(log_msg);
}

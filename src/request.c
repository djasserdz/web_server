#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "request.h"

void parse_request(char *buffer, ssize_t length, struct request *req)
{
    char *line_end = strstr(buffer, "\r\n");
    if (!line_end)
    {
        perror("malformer request");
        return;
    }
    int line_length = line_end - buffer;
    char request_line[512];
    strncpy(request_line, buffer, line_length);
    request_line[line_length] = '\0';

    sscanf(request_line, "%s %s %s", req->method, req->path, req->http_version);

    char *header_start = line_end + 2;

    int header_count = 0;
    while (header_start < buffer + length && header_count < 20)
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
            strncpy(req->headers[header_count][0], line, 255);
            strncpy(req->headers[header_count][1], colon + 2, 255);
            header_count++;
        }

        header_start = header_end + 2;
    }
    req->header_count = header_count;
}

#ifndef REQUEST_H
#define REQUEST_H

#include <sys/types.h>

struct request
{
    char method[8];
    char path[256];
    char http_version[10];
    char headers[20][2][256];
    int header_count;
};

void parse_request(char *buffer, ssize_t length, struct request *req);

#endif
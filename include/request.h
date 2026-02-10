#ifndef REQUEST_H
#define REQUEST_H

#include <sys/types.h>

typedef struct Header
{
    char *key;
    char *value;
} Header;
struct request
{
    char method[8];
    char path[256];
    char http_version[10];
    Header *Host;
    Header *Content_type;
    Header *Content_length;
    char *body;
    long body_length;
    int header_count;
    int invalid_method;
    int invalid_version;
};

void parse_request(char *buffer, ssize_t length, struct request *req);
int is_valid(const char *value, const char *list[], int size);
#endif
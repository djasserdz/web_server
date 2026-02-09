#ifndef RESPONSE_H
#define RESPONSE_H

typedef struct Header
{
    char *key;
    char *value;
} Header;

struct response
{
    char http_version[10];
    int status_code;
    char message[20];
    Header *Server;
    Header *Date;
    Header *Content_type;
    Header *Content_length;
};

void send_response(int client_fd, struct response *res, const char *body, long body_size, const char *content_type);
void set_response(struct response *res, int status_code);

#endif

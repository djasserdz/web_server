#ifndef RESPONSE_H
#define RESPONSE_H

struct response
{
    char http_version[10];
    int status_code;
    char message[20];
};

void send_response(int client_fd, struct response *res, const char *body);

#endif

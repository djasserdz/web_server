#ifndef PROXY_H
#define PROXY_H

#include <stddef.h>
#include <sys/types.h>

int open_proxy_connection(const char *ip, int port);

ssize_t send_proxy_request(int sockfd, const char *request, size_t length);

ssize_t read_proxy_response(int sockfd, int client_fd);

#endif // PROXY_H

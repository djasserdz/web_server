#include "config.h"
#include "request.h"
#include "response.h"
#include "handler.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define BUFFER_SIZE 8192

int create_server_socket(char *ip, int port)
{
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1)
    {
        perror("socker failed !");
        return -1;
    }
    struct sockaddr_in adress;
    adress.sin_family = AF_INET;
    // adress.sin_addr.s_addr = INADDR_ANY;
    adress.sin_port = htons(port);
    inet_pton(AF_INET, ip, &adress.sin_addr);

    if (bind(server, (struct sockaddr *)&adress, sizeof(adress)) < 0)
    {
        perror("bind failed!");
        return -1;
    }

    if (listen(server, 10) < 0)
    {
        perror("listen failed");
        return -1;
    }
    printf("Server listening on port %d\n", port);
    return server;
}

void set_non_blocking(int sockfd)
{
    int flags;
    flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("no flag is set");
        return;
    }

    flags |= O_NONBLOCK;
    if (fcntl(sockfd, F_SETFL, flags) == -1)
    {
        perror("error setting flags");
        return;
    }
}

void start_server(int server_fd)
{
    set_non_blocking(server_fd);

    while (1)
    {
        int client = accept(server_fd, NULL, NULL);

        if (client < 0)
        {
            if (errno != EAGAIN && errno != EWOULDBLOCK)
                perror("accept failed");
            continue;
        }

        set_non_blocking(client);

        char buffer[BUFFER_SIZE];
        size_t received_total = 0;
        ssize_t n;

        struct request req;
        memset(&req, 0, sizeof(req));
        struct response res;
        memset(&res, 0, sizeof(res));
        int headers_complete = 0;
        long content_length = 0;

        while (1)
        {
            n = recv(client, buffer + received_total, sizeof(buffer) - received_total, 0);
            if (n > 0)
            {
                received_total += n;

                if (!headers_complete)
                {
                    char *body_start = strstr(buffer, "\r\n\r\n");
                    if (body_start)
                    {
                        headers_complete = 1;
                        parse_request(buffer, received_total, &req);

                        if (req.invalid_method)
                        {
                            set_response(&res, 405);
                            send_response(client, &res, "<h1>405 Method Not Allowed</h1>", 28, "text/html");
                            close(client);
                            continue;
                        }

                        if (req.invalid_version)
                        {
                            set_response(&res, 505);
                            send_response(client, &res, "<h1>505 HTTP Version Not Supported</h1>", 40, "text/html");
                            close(client);
                            continue;
                        }

                        if (req.Content_length && req.Content_length->value)
                        {
                            content_length = atol(req.Content_length->value);
                        }

                        if (content_length == 0 || (received_total - (body_start - buffer + 4)) >= content_length)
                            break;
                    }
                }
                else
                {
                    char *body_start = strstr(buffer, "\r\n\r\n") + 4;
                    long body_received = received_total - (body_start - buffer);
                    if (body_received >= content_length)
                        break;
                }
            }
            else if (n == 0)
            {
                close(client);
                break;
            }
            else if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                continue;
            }
            else
            {
                perror("recv failed");
                close(client);
                break;
            }
        }

        if (received_total == 0)
            continue;

        if (req.Content_length && content_length > 0)
        {
            char *body_start = strstr(buffer, "\r\n\r\n") + 4;
            req.body_length = received_total - (body_start - buffer);
            req.body = malloc(content_length);
            if (!req.body)
            {
                perror("malloc failed");
                close(client);
                continue;
            }
            memcpy(req.body, body_start, req.body_length);
        }
        handle_request(client, &req);

        if (req.body)
            free(req.body);
        close(client);
    }
}

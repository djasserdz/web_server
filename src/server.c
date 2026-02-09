#include "config.h"
#include "request.h"
#include "response.h"
#include "handler.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

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

        if (client >= 0)
        {
            char buffer[1024];
            ssize_t bytes = recv(client, buffer, sizeof(buffer), 0);

            if (bytes > 0)
            {
                struct request req;
                parse_request(buffer, bytes, &req);

                struct response res;
                handle_request(client, &req);
            }
            else if (bytes == 0)
            {
                close(client);
            }
            else if (bytes == -1 && errno == EAGAIN)
            {
            }
            else
            {
                perror("recv failed");
                close(client);
            }

            close(client);
        }
        else if (client == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            continue;
        }
        else
        {
            perror("accept failed");
        }
    }
}

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

int open_proxy_connection(char *ip, int port)
{
    int proxy_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (proxy_socket == -1)
    {
        perror("proxy failed");
        return -1;
    }
    struct sockaddr_in adress;
    adress.sin_family = AF_INET;
    adress.sin_port = htons(port);
    inet_pton(AF_INET, ip, &adress.sin_addr);
    if (connect(proxy_socket, (struct sockaddr *)&adress, sizeof(adress)) < 0)
    {
        perror("not connected");
        close(proxy_socket);
        return -1;
    }
    return proxy_socket;
}

ssize_t send_proxy_request(int sockfd, const char *data, size_t len)
{
    return send(sockfd, data, len, 0);
}

ssize_t read_proxy_response(int sockfd, int client_fd)
{
    char buffer[4096];
    ssize_t n;
    ssize_t total_forwarded = 0;

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));

    while ((n = read(sockfd, buffer, sizeof(buffer))) > 0)
    {
        ssize_t sent = 0;
        while (sent < n)
        {
            ssize_t s = send(client_fd, buffer + sent, n - sent, 0);
            if (s < 0)
            {
                perror("send failed");
                return -1;
            }
            sent += s;
            total_forwarded += s;
        }
    }

    if (n < 0)
    {
        perror("read from backend failed");
        return -1;
    }

    return total_forwarded;
}

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include "server.h"
#include "request.h"
#include "response.h"

int main()
{

    Config config;
    if (read_config("config.cfg", &config) != 0)
    {
        printf("Failed to read config file\n");
        return 1;
    }

    printf("Server will run on %s:%d\n", config.ip, config.port);
    if (config.proxy_enabled)
        printf("Proxy forwarding enabled to %s:%d\n", config.proxy_host, config.proxy_port);

    int server_fd = create_server_socket(config.ip, config.port);
    if (server_fd < 0)
    {
        perror("Failed to create server socket");
        return EXIT_FAILURE;
    }

    start_server(server_fd);

    return 0;
}

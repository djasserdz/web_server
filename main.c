#include <stdio.h>
#include <stdlib.h>
#include "server.h"
#include "request.h"
#include "response.h"

int main()
{
    int server_fd = create_server_socket(8080);
    if (server_fd < 0)
    {
        perror("Failed to create server socket");
        return EXIT_FAILURE;
    }

    start_server(server_fd);

    return 0;
}

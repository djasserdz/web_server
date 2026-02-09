#ifndef SERVER_H
#define SERVER_H

int create_server_socket(int port);
void set_non_blocking(int sockfd);
void start_server(int server_fd);

#endif

int accept_connection(int server);
void close_connection(int client);
ssize_t read_from_client(int client,char *buffer,size_t size);
ssize_t write_to_client(int clinet,char *buffer,size_t size);
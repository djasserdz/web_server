#ifndef HANDLER_H
#define HANDLER_H

#include "response.h"
#include "request.h"

void handle_request(int client_fd, struct request *req);
char *serve_static_file(const char *path, struct response *res, long *out_size);
const char *get_mime_type(const char *path);

#endif
#include "handler.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define STATIC_DIR "static"

void handle_request(int client_fd, struct request *req)
{
    struct response res;
    strcpy(res.http_version, req->http_version);
    Header server = {"Server", "MyCServer/0.1"};
    Header date = {"Date",
                   NULL};
    res.Server = &server;
    res.Date = &date;
    long body_size = 0;
    char *body = serve_static_file(req->path, &res, &body_size);

    if (body)
    {
        const char *mimes = get_mime_type(req->path);
        send_response(client_fd, &res, body, body_size, mimes);
        free(body);
    }
    else
    {
        {
            set_response(&res, 404);

            const char *content_type = "text/html";

            const char *not_found_body = "<h1>404 Not Found</h1>";

            send_response(client_fd, &res, not_found_body, strlen(not_found_body), content_type);
        }
    }
}

char *serve_static_file(const char *path, struct response *res, long *out_size)
{
    const char *ext = strrchr(path, '.');
    if (!ext)
    {
        return NULL;
    }

    char file_path[512];
    snprintf(file_path, sizeof(file_path), "%s%s", STATIC_DIR, path);

    FILE *fp = fopen(file_path, "rb");
    if (!fp)
    {
        set_response(res, 404);
        *out_size = 0;
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *buffer = malloc(file_size);
    fread(buffer, 1, file_size, fp);
    fclose(fp);

    set_response(res, 200);
    *out_size = file_size;
    return buffer;
}

const char *get_mime_type(const char *path)
{
    const char *ext = strrchr(path, '.');
    if (!ext)
        return "text/plain";
    if (strcmp(ext, ".html") == 0)
        return "text/html";
    if (strcmp(ext, ".css") == 0)
        return "text/css";
    if (strcmp(ext, ".js") == 0)
        return "application/javascript";
    if (strcmp(ext, ".png") == 0)
        return "image/png";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0)
        return "image/jpeg";
    if (strcmp(ext, ".gif") == 0)
        return "image/gif";
    return "application/octet-stream";
}

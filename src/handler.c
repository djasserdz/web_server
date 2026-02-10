#include "handler.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include <sys/types.h>
#include <sys/socket.h>
#include "proxy.h"

#define STATIC_DIR "static"

#include "proxy.h"
#include "config.h"

void handle_request(int client_fd, struct request *req)
{
    Config config;
    if (read_config("config.cfg", &config) != 0)
    {
        printf("Failed to read config file\n");
        return;
    }

    struct response res;
    strcpy(res.http_version, req->http_version);

    Header server = {"Server", "MyCServer/0.1"};
    Header date = {"Date", NULL};
    res.Server = &server;
    res.Date = &date;

    long body_size = 0;
    char *body = serve_static_file(req->path, &res, &body_size);

    if (body)
    {
        const char *mime = get_mime_type(req->path);
        send_response(client_fd, &res, body, body_size, mime);
        free(body);
        return;
    }

    if (config.proxy_enabled)
    {
        int proxy_sock = open_proxy_connection(config.proxy_host, config.proxy_port);
        if (proxy_sock < 0)
        {
            set_response(&res, 500);
            send_response(client_fd, &res,
                          "<h1>Proxy Error</h1>",
                          strlen("<h1>Proxy Error</h1>"),
                          "text/html");
            return;
        }

        char request_buf[8192];
        int len = snprintf(
            request_buf, sizeof(request_buf),
            "%s %s %s\r\n"
            "Host: %s\r\n"
            "Content-Length: %ld\r\n"
            "Connection: close\r\n\r\n",
            req->method,
            req->path,
            req->http_version,
            req->Host ? req->Host->value : "localhost",
            req->body_length);

        if (req->body_length > 0 && req->body)
        {
            memcpy(request_buf + len, req->body, req->body_length);
            len += req->body_length;
        }

        send_proxy_request(proxy_sock, request_buf, len);
        read_proxy_response(proxy_sock, client_fd);

        close(proxy_sock);
        return;
    }

    set_response(&res, 404);
    send_response(client_fd, &res,
                  "<h1>404 Not Found</h1>",
                  strlen("<h1>404 Not Found</h1>"),
                  "text/html");
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

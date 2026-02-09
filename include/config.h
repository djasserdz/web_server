#ifndef CONFIG_H
#define CONFIG_H

typedef struct Config
{
    char ip[16];
    int port;
    int proxy_enabled;
    char proxy_host[16];
    int proxy_port;
} Config;

int read_config(const char *filename, Config *config);

#endif

#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int read_config(const char *filename, Config *config)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return -1;

    char line[256];
    while (fgets(line, sizeof(line), fp))
    {
        if (line[0] == '#' || line[0] == '\n')
            continue;

        char key[64], value[128];
        if (sscanf(line, "%63[^=]=%127s", key, value) == 2)
        {
            if (strcmp(key, "IP") == 0)
                strncpy(config->ip, value, sizeof(config->ip));
            else if (strcmp(key, "PORT") == 0)
                config->port = atoi(value);
            else if (strcmp(key, "PROXY_ENABLED") == 0)
                config->proxy_enabled = atoi(value);
            else if (strcmp(key, "PROXY_HOST") == 0)
                strncpy(config->proxy_host, value, sizeof(config->proxy_host));
            else if (strcmp(key, "PROXY_PORT") == 0)
                config->proxy_port = atoi(value);
        }
    }

    fclose(fp);
    return 0;
}

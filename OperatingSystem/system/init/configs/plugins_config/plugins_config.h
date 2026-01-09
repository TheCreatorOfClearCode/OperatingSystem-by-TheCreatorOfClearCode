#pragma once

typedef void (*plugin_func_t)();

typedef enum
{
    PLUGIN_TYPE_MODULE,
    PLUGIN_TYPE_COMMAND
} PluginType;

typedef struct
{
    const char *name;
    PluginType type;
    plugin_func_t start;
    plugin_func_t stop;
    plugin_func_t init;
    const char *description;
} PluginConfig;

extern PluginConfig plugins[];
extern int plugins_count;
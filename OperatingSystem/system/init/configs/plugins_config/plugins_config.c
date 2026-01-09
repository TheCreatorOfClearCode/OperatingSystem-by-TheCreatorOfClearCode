#include "plugins_config.h"
#include "../../../../plugins/shell/command_interpreter/command_interpreter.h"
#include"../../../../plugins/shell/shell/shell.h"

extern void about_init();
extern void help_init();
extern void clear_init();
extern void clear_history_init();
extern void power_controller_init();
extern void datetime_init();
extern void installed_init();

PluginConfig plugins[] = {
    {
        .name = "Shell",
        .type = PLUGIN_TYPE_MODULE,
        .start = shell_run,
        .stop = shell_stop,
        .init = 0,
        .description = "Main system shell for user input and command execution"
    },
    {
        .name = "about",
        .type = PLUGIN_TYPE_COMMAND,
        .start = 0,
        .stop = 0,
        .init = about_init,
        .description = "Plug-in for displaying information about the system"
    },
    {
        .name = "help",
        .type = PLUGIN_TYPE_COMMAND,
        .start = 0,
        .stop = 0,
        .init = help_init,
        .description = "Plug-in for displaying available commands"
    },
    {
        .name = "clear",
        .type = PLUGIN_TYPE_COMMAND,
        .start = 0,
        .stop = 0,
        .init = clear_init,
        .description = "Plug-in for clearing the screen or commands history"
    },
    {
        .name = "power",
        .type = PLUGIN_TYPE_COMMAND,
        .start = 0,
        .stop = 0,
        .init = power_controller_init,
        .description = "Plug-in for power management"
    },
    {
        .name = "date&time",
        .type = PLUGIN_TYPE_COMMAND,
        .start = 0,
        .stop = 0,
        .init = datetime_init,
        .description = "Plug-in for displaying date and time"
    },
    {
        .name = "installed",
        .type = PLUGIN_TYPE_COMMAND,
        .start = 0,
        .stop = 0,
        .init = installed_init,
        .description = "Plug-in for displaying installed drivers and plugins"
    }
};

int plugins_count = sizeof(plugins) / sizeof(plugins[0]);
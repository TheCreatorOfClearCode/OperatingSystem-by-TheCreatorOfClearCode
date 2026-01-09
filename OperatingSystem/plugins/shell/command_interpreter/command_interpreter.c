#include "command_interpreter.h"
#include "../../../system/kernel/drivers/VGA/vga.h"
#include "../../../libraries/string/string.h"

#define MAX_COMMANDS 32
Command commands[MAX_COMMANDS];
size_t command_count = 0;

void register_command(const char *name, void (*func)(const char *args), const char *description, const char *usage, int accepts_args)
{
    for (size_t i = 0; i < command_count; i++)
    {
        if (!strcmp(commands[i].name, name))
            return;
    }

    if (command_count < MAX_COMMANDS)
    {
        commands[command_count].name = name;
        commands[command_count].func = func;
        commands[command_count].description = description;
        commands[command_count].usage = usage;
        commands[command_count].accepts_args = accepts_args;
        command_count++;
    }
}

void execute_command_by_index(size_t index, const char *args)
{
    if (index < command_count)
    {
        commands[index].func(args);
    }
}

static int find_command(const char *cmd_start, const char **args_start)
{
    int best_match = -1;
    size_t best_match_len = 0;

    for (size_t i = 0; i < command_count; i++)
    {
        const char *cmd_name = commands[i].name;
        size_t cmd_len = strlen(cmd_name);

        if (strncmp(cmd_start, cmd_name, cmd_len) == 0)
        {
            char next_char = cmd_start[cmd_len];
            if (next_char == '\0' || next_char == ' ')
            {
                if (cmd_len > best_match_len)
                {
                    best_match = i;
                    best_match_len = cmd_len;
                    *args_start = cmd_start + cmd_len;
                }
            }
        }
    }

    return best_match;
}

static int validate_args(int cmd_index, const char *args)
{
    if (cmd_index == -1)
        return 0;

    if (!commands[cmd_index].accepts_args && args != NULL && strlen(args) > 0)
    {
        vga_write_color("Error: command '", COLOR_RED, COLOR_BLACK);
        vga_write_color(commands[cmd_index].name, COLOR_LIGHT_RED, COLOR_BLACK);
        vga_write_color("' does not accept arguments\n", COLOR_RED, COLOR_BLACK);

        if (commands[cmd_index].usage != NULL)
        {
            vga_write_color("Usage: ", COLOR_LIGHT_GRAY, COLOR_BLACK);
            vga_write_color(commands[cmd_index].usage, COLOR_WHITE, COLOR_BLACK);
            vga_putc('\n');
        }

        return 0;
    }

    return 1;
}

void execute_command(const char *buf)
{
    char command_buf[256];
    char unknown_commands[8][32];
    size_t unknown_count = 0;

    while (*buf)
    {
        size_t pos = 0;

        while (*buf && !(buf[0] == '&' && buf[1] == '&') && pos < sizeof(command_buf) - 1)
        {
            command_buf[pos++] = *buf++;
        }
        command_buf[pos] = '\0';

        if (*buf == '&' && *(buf + 1) == '&')
            buf += 2;

        while (*buf == ' ')
            buf++;

        char *cmd_ptr = command_buf;
        while (*cmd_ptr == ' ')
            cmd_ptr++;

        if (*cmd_ptr == '\0')
            continue;

        const char *args = NULL;
        int cmd_index = find_command(cmd_ptr, &args);

        if (cmd_index != -1)
        {
            while (args && *args == ' ')
                args++;

            if (!validate_args(cmd_index, args))
            {
                continue;
            }

            if (args && *args == '\0')
                args = NULL;

            commands[cmd_index].func(args);
        }
        else
        {
            char unknown_cmd[32];
            size_t i = 0;
            while (cmd_ptr[i] && cmd_ptr[i] != ' ' && i < sizeof(unknown_cmd) - 1)
            {
                unknown_cmd[i] = cmd_ptr[i];
                i++;
            }
            unknown_cmd[i] = '\0';

            if (unknown_count < 8)
            {
                strcpy(unknown_commands[unknown_count++], unknown_cmd);
            }
        }
    }

    for (size_t k = 0; k < unknown_count; k++)
    {
        vga_write_color("Unknown command: ", COLOR_RED, COLOR_BLACK);
        vga_write_color(unknown_commands[k], COLOR_RED, COLOR_BLACK);
        vga_putc('\n');
    }
}
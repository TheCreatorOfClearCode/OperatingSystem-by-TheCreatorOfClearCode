#include "../../../system/kernel/drivers/VGA/vga.h"
#include "../../../system/kernel/drivers/keyboard/keyboard.h"
#include "../commands_execute/commands_execute.h"
#include "../../../libraries/string/string.h"
#include "shell.h"

#define HISTORY_SIZE 16

static int shell_running = 0;

static char history[HISTORY_SIZE][128];
static size_t history_count = 0;

void shell_run()
{
    shell_running = 1;

    char buf[128];
    int history_index = -1;

    vga_clear();
    vga_write_color_centered("Welcome!", COLOR_YELLOW, COLOR_BLACK);
    vga_putc('\n');

    vga_write_color_centered("Type 'help' to get a list of available commands", COLOR_LIGHT_GRAY, COLOR_BLACK);
    vga_write_color_centered("Use ↑/↓ to navigate through the command history", COLOR_LIGHT_GRAY, COLOR_BLACK);
    vga_putc('\n');
    vga_putc('\n');

    while (shell_running)
    {
        vga_write_color("-> ", COLOR_GREEN, COLOR_BLACK);
        size_t len = 0;
        history_index = history_count;

        while (shell_running)
        {
            int c = keyboard_getkey();

            if (!shell_running)
                break;

            if (c >= 256)
            {
                switch (c)
                {
                case KEY_UP:
                    if (history_count > 0 && history_index > 0)
                    {
                        while (len > 0)
                        {
                            len--;
                            size_t r = vga_get_row();
                            size_t c_pos = vga_get_col();
                            if (c_pos == 0 && r > 0)
                            {
                                r--;
                                c_pos = 79;
                            }
                            else
                                c_pos--;
                            vga_clear_char_at(r, c_pos);
                            vga_set_row_col(r, c_pos);
                        }

                        history_index--;
                        strcpy(buf, history[history_index]);
                        len = strlen(buf);
                        vga_write(buf);
                    }
                    continue;

                case KEY_DOWN:
                    if (history_index < (int)history_count - 1)
                    {
                        while (len > 0)
                        {
                            len--;
                            size_t r = vga_get_row();
                            size_t c_pos = vga_get_col();
                            if (c_pos == 0 && r > 0)
                            {
                                r--;
                                c_pos = 79;
                            }
                            else
                                c_pos--;
                            vga_clear_char_at(r, c_pos);
                            vga_set_row_col(r, c_pos);
                        }

                        history_index++;
                        strcpy(buf, history[history_index]);
                        len = strlen(buf);
                        vga_write(buf);
                    }
                    else if (history_index == (int)history_count - 1)
                    {
                        while (len > 0)
                        {
                            len--;
                            size_t r = vga_get_row();
                            size_t c_pos = vga_get_col();
                            if (c_pos == 0 && r > 0)
                            {
                                r--;
                                c_pos = 79;
                            }
                            else
                                c_pos--;
                            vga_clear_char_at(r, c_pos);
                            vga_set_row_col(r, c_pos);
                        }
                        len = 0;
                        buf[0] = 0;
                        history_index = history_count;
                    }
                    continue;

                case KEY_LEFT:
                case KEY_RIGHT:
                    continue;

                default:
                    continue;
                }
            }

            if (c == '\n' || c == '\r')
            {
                buf[len] = 0;
                vga_putc('\n');

                if (len > 0)
                {
                    if (history_count < HISTORY_SIZE)
                        strcpy(history[history_count++], buf);
                    else
                    {
                        for (int i = 1; i < HISTORY_SIZE; i++)
                            strcpy(history[i - 1], history[i]);
                        strcpy(history[HISTORY_SIZE - 1], buf);
                    }
                }

                if (strcmp(buf, "exit") == 0)
                {
                    vga_write_color("Exiting shell...\n", COLOR_WHITE, COLOR_BLACK);
                    shell_stop();
                    break;
                }

                execute_command(buf);
                break;
            }

            else if (c == '\b')
            {
                if (len > 0)
                {
                    len--;
                    size_t r = vga_get_row();
                    size_t c_pos = vga_get_col();

                    if (c_pos == 0 && r > 0)
                    {
                        r--;
                        c_pos = 79;
                    }
                    else
                    {
                        c_pos--;
                    }

                    vga_clear_char_at(r, c_pos);
                    vga_set_row_col(r, c_pos);
                }
            }

            else if (c == 27)
            {
                int next = keyboard_getkey();
                if (next == '[')
                {
                    keyboard_getkey();
                }
                continue;
            }
            else if (c && len < 127)
            {
                buf[len++] = c;
                vga_putc(c);
            }
        }
    }
}

void shell_stop()
{
    shell_running = 0;
}

int shell_is_running()
{
    return shell_running;
}

void clear_command(const char *args)
{
    if (args == NULL || strlen(args) == 0)
    {
        vga_write_color("Usage: clear <argument>\n\n", COLOR_LIGHT_GRAY, COLOR_BLACK);
        vga_write_color("Available arguments:\n", COLOR_LIGHT_CYAN, COLOR_BLACK);
        vga_write_color("  screen  - Clear the screen\n", COLOR_WHITE, COLOR_BLACK);
        vga_write_color("  history - Clear command history\n", COLOR_WHITE, COLOR_BLACK);
        vga_putc('\n');
        return;
    }

    char arg[32];
    int i = 0;

    while (*args == ' ')
        args++;

    while (*args && *args != ' ' && i < sizeof(arg) - 1)
    {
        arg[i++] = *args++;
    }
    arg[i] = '\0';

    if (strcmp(arg, "screen") == 0)
    {
        vga_clear();
    }
    else if (strcmp(arg, "history") == 0)
    {
        for (size_t i = 0; i < HISTORY_SIZE; i++)
            history[i][0] = '\0';
        history_count = 0;

        vga_write_color("Command history cleared!\n", COLOR_GREEN, COLOR_BLACK);
    }
    else
    {
        vga_write_color("Error: Unknown argument '", COLOR_RED, COLOR_BLACK);
        vga_write_color(arg, COLOR_LIGHT_RED, COLOR_BLACK);
        vga_write_color("'\n", COLOR_RED, COLOR_BLACK);
        vga_write_color("Use 'clear' without arguments to see available options.\n", COLOR_LIGHT_GRAY, COLOR_BLACK);
    }
}

void clear_init()
{
    register_command("clear", clear_command, "Clear screen or command history", "clear [screen|history]", 1);
}
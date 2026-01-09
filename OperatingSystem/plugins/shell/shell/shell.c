#include "../../../system/kernel/drivers/VGA/vga.h"
#include "../../../system/kernel/drivers/keyboard/keyboard.h"
#include "../command_interpreter/command_interpreter.h"
#include "../../../libraries/string/string.h"
#include "shell.h"

#define HISTORY_SIZE 16
#define INPUT_MAX_COLS (VGA_WIDTH - input_col - 1)

static int shell_running = 0;

static char history[HISTORY_SIZE][128];
static size_t history_count = 0;
static char pending_input[128] = {0};
static int has_pending_input = 0;

static void redraw_input(const char *buf, size_t len, size_t input_row, size_t input_col) 
{
    for (size_t i = 0; i < INPUT_MAX_COLS; i++)
    {
        char ch = (i < len) ? buf[i] : ' ';
        vga_putc_at(input_row, input_col + i, ch);
    }

    move_cursor();
}

static void move_input_cursor(size_t input_row, size_t input_col, size_t cursor) 
{
    vga_set_row_col(input_row, input_col + cursor);
}

void shell_run()
{
    shell_running = 1;

    char buf[128];
    int history_index = -1;
    size_t len = 0;
    size_t cursor = 0;

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

        size_t input_row = vga_get_row();
        size_t input_col = vga_get_col();

        len = 0;
        cursor = 0;
        buf[0] = '\0';
        
        if (has_pending_input && pending_input[0] != '\0')
        {
            strcpy(buf, pending_input);
            len = strlen(buf);
            cursor = len;
            vga_write(buf);
            has_pending_input = 0;
        }

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
                    if (history_count > 0)
                    {
                        if (len > 0 && history_index == history_count)
                        {
                            strcpy(pending_input, buf);
                            has_pending_input = 1;
                        }

                        if (history_index > 0)
                        {
                            history_index--;
                            strcpy(buf, history[history_index]);
                            len = strlen(buf);
                            cursor = len;
                            redraw_input(buf, len, input_row, input_col);
                            move_input_cursor(input_row, input_col, cursor);
                        }
                    }
                    continue;

                case KEY_DOWN:
                    if (history_index < (int)history_count - 1)
                    {
                        history_index++;
                        strcpy(buf, history[history_index]);
                        len = strlen(buf);
                        cursor = len;
                        redraw_input(buf, len, input_row, input_col);
                        move_input_cursor(input_row, input_col, cursor);
                    }
                    else if (history_index == (int)history_count - 1)
                    {
                        history_index = history_count;
                        
                        if (has_pending_input)
                        {
                            strcpy(buf, pending_input);
                            len = strlen(buf);
                            has_pending_input = 0;
                        }
                        else
                        {
                            buf[0] = '\0';
                            len = 0;
                        }
                        
                        cursor = len;
                        redraw_input(buf, len, input_row, input_col);
                        move_input_cursor(input_row, input_col, cursor);
                    }
                    continue;

                case KEY_LEFT:
                    if (cursor > 0) 
                    {
                        cursor--;
                        move_input_cursor(input_row, input_col, cursor);
                    }
                    continue;

                case KEY_RIGHT:
                    if (cursor < len && cursor < INPUT_MAX_COLS) 
                    {
                        cursor++;
                        move_input_cursor(input_row, input_col, cursor);
                    }
                    continue;

                case KEY_DELETE:
                    if (cursor < len) 
                    {
                        for (size_t i = cursor; i < len - 1; i++)
                            buf[i] = buf[i + 1];

                        len--;
                        buf[len] = '\0';

                        redraw_input(buf, len, input_row, input_col);
                        move_input_cursor(input_row, input_col, cursor);
                    }
                    continue;

                default:
                    continue;
                }
            }

            if (c == '\n' || c == '\r')
            {
                buf[len] = '\0';
                vga_putc('\n');

                int has_content = 0;
                for (size_t i = 0; i < len; i++) {
                    if (buf[i] != ' ') {
                        has_content = 1;
                        break;
                    }
                }

                if (has_content)
                {
                    int already_in_history = 0;
                    for (size_t i = 0; i < history_count; i++) {
                        if (strcmp(history[i], buf) == 0) {
                            already_in_history = 1;
                            break;
                        }
                    }
                    
                    if (!already_in_history) {
                        if (history_count < HISTORY_SIZE) {
                            strcpy(history[history_count++], buf);
                        } else {
                            for (int i = 1; i < HISTORY_SIZE; i++)
                                strcpy(history[i - 1], history[i]);
                            strcpy(history[HISTORY_SIZE - 1], buf);
                        }
                    }
                }

                pending_input[0] = '\0';
                has_pending_input = 0;

                if (strcmp(buf, "exit") == 0)
                {
                    vga_write_color("Exiting shell...\n", COLOR_WHITE, COLOR_BLACK);
                    shell_stop();
                    break;
                }

                if (has_content) {
                    execute_command(buf);
                }
                break;
            }

            else if (c == '\b')
            {
                if (cursor > 0 && len > 0) 
                {
                    for (size_t i = cursor - 1; i < len - 1; i++)
                        buf[i] = buf[i + 1];

                    cursor--;
                    len--;
                    buf[len] = '\0';

                    redraw_input(buf, len, input_row, input_col);
                    move_input_cursor(input_row, input_col, cursor);
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
            else if (c && c >= 32 && len < 127 && len < INPUT_MAX_COLS)
            {
                for (size_t i = len; i > cursor; i--)
                    buf[i] = buf[i - 1];

                buf[cursor] = (char)c;
                len++;
                cursor++;
                buf[len] = '\0';

                redraw_input(buf, len, input_row, input_col);
                move_input_cursor(input_row, input_col, cursor);
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
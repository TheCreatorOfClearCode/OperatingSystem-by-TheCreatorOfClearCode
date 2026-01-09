#include "../libraries/string/string.h"
#include "shell/command_interpreter/command_interpreter.h"
#include "../system/kernel/drivers/VGA/vga.h"
#include "../system/kernel/drivers/keyboard/keyboard.h"

#define COMMANDS_PER_PAGE 5
#define HEADER_ROWS 3
#define COMMANDS_START_ROW 4

static int help_total_pages = 0;
static int help_current_page = 0;
static int help_is_active = 0;

static void sort_commands_alphabetically()
{
    for (size_t i = 0; i < command_count; i++)
    {
        for (size_t j = i + 1; j < command_count; j++)
        {
            if (strcasecmp(commands[i].name, commands[j].name) > 0)
            {
                Command temp = commands[i];
                commands[i] = commands[j];
                commands[j] = temp;
            }
        }
    }
}

static void draw_help_static_elements()
{
    vga_disable_cursor();
    
    vga_set_row_col(0, 0);
    
    const char *top_frame = "╔══════════════════════════════════════════════╗";
    const char *title_frame = "║                Command List                  ║";
    const char *bottom_frame = "╚══════════════════════════════════════════════╝";

    vga_write_color_centered(top_frame, COLOR_LIGHT_GRAY, COLOR_BLACK);
    vga_write_color_centered(title_frame, COLOR_LIGHT_GRAY, COLOR_BLACK);
    vga_write_color_centered(bottom_frame, COLOR_LIGHT_GRAY, COLOR_BLACK);

    vga_set_row_col(COMMANDS_START_ROW + COMMANDS_PER_PAGE, 0);
    const char *separator = "────────────────────────────────────────────────────────────────────────────────";
    vga_write_color_centered(separator, COLOR_DARK_GRAY, COLOR_BLACK);
}

static void draw_help_dynamic_elements()
{
    vga_clear_area(COMMANDS_START_ROW, COMMANDS_START_ROW + COMMANDS_PER_PAGE - 1);

    vga_set_row_col(COMMANDS_START_ROW, 0);

    size_t start = help_current_page * COMMANDS_PER_PAGE;
    size_t end = start + COMMANDS_PER_PAGE;
    if (end > command_count)
        end = command_count;

    if (command_count == 0)
    {
        vga_write_color("No commands available.\n", COLOR_LIGHT_RED, COLOR_BLACK);
    }
    else
    {
        for (size_t i = start; i < end; i++)
        {
            vga_write_color(commands[i].name, COLOR_CYAN, COLOR_BLACK);
            vga_write_color(" - ", COLOR_LIGHT_GRAY, COLOR_BLACK);
            vga_write_color(commands[i].description, COLOR_WHITE, COLOR_BLACK);
            vga_putc('\n');
        }
    }

    vga_clear_area(COMMANDS_START_ROW + COMMANDS_PER_PAGE + 1, COMMANDS_START_ROW + COMMANDS_PER_PAGE + 1);
    vga_set_row_col(COMMANDS_START_ROW + COMMANDS_PER_PAGE + 1, 0);
    char footer[64];
    str_format(footer, sizeof(footer), "[Page %d/%d]  ←/→  Navigate   ESC  Exit", help_current_page + 1, help_total_pages);
    vga_write_color_centered(footer, COLOR_LIGHT_GREEN, COLOR_BLACK);
}

static void draw_help_page()
{
    if (!help_is_active)
        return;

    draw_help_static_elements();
    draw_help_dynamic_elements();
}

void help_command()
{
    help_is_active = 1;
    sort_commands_alphabetically();

    help_total_pages = (command_count + COMMANDS_PER_PAGE - 1) / COMMANDS_PER_PAGE;
    help_current_page = 0;
    int running = 1;

    vga_clear();
    draw_help_page();

    while (running && help_is_active)
    {
        int key = keyboard_getkey();
        int old_page = help_current_page;

        if (key == KEY_RIGHT && help_current_page < help_total_pages - 1)
            help_current_page++;
        else if (key == KEY_LEFT && help_current_page > 0)
            help_current_page--;
        else if (key == KEY_ESC)
            running = 0;
        else
            continue;

        if (help_current_page != old_page)
        {
            draw_help_dynamic_elements();
        }
    }

    help_is_active = 0;
    vga_clear();
    vga_enable_cursor(14, 15);
}

void help_init()
{
    register_command("help", help_command, "Show list of available commands", "help", 0);
}
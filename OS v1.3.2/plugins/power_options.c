#include <stdint.h>
#include "shell/command_interpreter/command_interpreter.h"
#include "../system/kernel/drivers/VGA/vga.h"
#include "../system/init/init_system/init_system.h"
#include "../system/kernel/drivers/keyboard/keyboard.h"
#include "../libraries/string/string.h"

static const char *options[] = {"Shutdown", "Reboot"};
static int total_options = 2;

int running = 0;

void draw_power_menu(int current)
{
    vga_clear_area(5, VGA_HEIGHT - 1);
    vga_set_row_col(5, 0);

    for (int i = 0; i < total_options; i++)
    {
        uint8_t color_fg = (i == current) ? COLOR_YELLOW : COLOR_LIGHT_GRAY;

        char line[64];
        strcpy(line, "[");
        strcat(line, options[i]);
        strcat(line, "]");

        vga_write_color_centered(line, color_fg, COLOR_BLACK);
    }
}

void power_controller_command()
{
    int current = 0;
    running = 1;
    vga_clear();
    vga_disable_cursor();

    vga_write_color_centered("=== Power options ===\n", COLOR_GREEN, COLOR_BLACK);
    vga_write_color_centered("Use ↑/↓ to select, Enter to execute, ESC to exit\n\n", COLOR_LIGHT_GRAY, COLOR_BLACK);

    draw_power_menu(current);

    while (running)
    {
        int key = keyboard_getkey();
        int prev = current;

        if (key == KEY_UP && current > 0)
            current--;
        else if (key == KEY_DOWN && current < total_options - 1)
            current++;

        if (current != prev)
            draw_power_menu(current);

        else if (key == '\n')
        {
            running = 0;
            if (current == 0)
                system_poweroff();
            else
                system_reboot();
        }
        else if (key == KEY_ESC)
        {
            running = 0;
        }
    }

    vga_clear();
    vga_enable_cursor(14, 15);
}

void power_controller_init()
{
    register_command("power", power_controller_command, "Open power options menu", "power", 0);
}
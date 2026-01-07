#include <stdint.h>
#include "../libraries/string/string.h"
#include "shell/commands_execute/commands_execute.h"
#include "../system/kernel/drivers/VGA/vga.h"
#include "../system/kernel/drivers/keyboard/keyboard.h"
#include "../system/about_system/about_system.h"
#include "../system/about_system/os_info.h"

extern const char *KERNEL;
extern uint32_t total_memory_mb;

static void print_num(uint32_t n)
{
    char buf[16];
    int i = 0;
    if (n == 0)
        buf[i++] = '0';
    while (n > 0)
    {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }
    buf[i] = '\0';
    for (int j = i - 1; j >= 0; j--)
        vga_putc(buf[j]);
}

static void draw_tab_header(int current, int total, const char *titles[])
{
    int total_len = 0;
    for (int i = 0; i < total; i++)
        total_len += strlen(titles[i]) + 3;

    int start_col = (VGA_WIDTH - total_len) / 2;
    if (start_col < 0)
        start_col = 0;

    vga_set_row_col(4, start_col);

    for (int i = 0; i < total; i++)
    {
        if (i == current)
            vga_write_color("[", COLOR_YELLOW, COLOR_BLACK);
        else
            vga_write("[");

        vga_write_color(titles[i], (i == current) ? COLOR_LIGHT_GREEN : COLOR_WHITE, COLOR_BLACK);

        if (i == current)
            vga_write_color("]", COLOR_YELLOW, COLOR_BLACK);
        else
            vga_write("]");

        vga_write(" ");
    }
}

static void clear_tab_area()
{
    vga_clear_area(5, VGA_HEIGHT - 1);
}

void about_command()
{
    bios_info_t bios = {"Unknown", "Unknown", "Unknown", 0, 0, 0, 0, 0, 0};
    system_info_t sys = {"Unknown", "Unknown", "Unknown", "Unknown", "Unknown"};
    board_info_t board = {"Unknown", "Unknown", "Unknown", "Unknown"};
    about_get_smbios_info(&sys, &board, &bios);

    const char *tabs[] = {"OS", "CPU", "Memory", "System", "Board", "BIOS"};
    int total_tabs = 6, current = 0, prev_tab = -1, running = 1;

    vga_clear();
    vga_disable_cursor();

    vga_write_color_centered("=== About ===\n", COLOR_GREEN, COLOR_BLACK);
    vga_write_color_centered("Use ←/→ to switch tabs, ESC to exit\n", COLOR_LIGHT_GRAY, COLOR_BLACK);
    vga_putc('\n');
    draw_tab_header(current, total_tabs, tabs);

    while (running)
    {
        if (prev_tab != current)
        {
            clear_tab_area();

            vga_set_row_col(0, 0);
            draw_tab_header(current, total_tabs, tabs);

            vga_set_row_col(6, 0);

            switch (current)
            {
            case 0:
                vga_write_color("Operating System\n", COLOR_LIGHT_GREEN, COLOR_BLACK);
                vga_write_color(" Name: ", COLOR_WHITE, COLOR_BLACK);
                vga_write(OS_NAME);
                vga_write_color("\n Version: ", COLOR_WHITE, COLOR_BLACK);
                vga_write(OS_VERSION);
                vga_write_color("\n Kernel: ", COLOR_WHITE, COLOR_BLACK);
                vga_write(KERNEL);
                vga_write_color("\n Developer: ", COLOR_WHITE, COLOR_BLACK);
                vga_write(OS_DEVELOPER);
                break;

            case 1:
            {
                char brand[49];
                about_get_cpu_brand(brand);

                uint32_t logical_cores = 0, physical_cores = 0;
                uint32_t base_freq = 0, l1 = 0, l2 = 0, l3 = 0;
                about_get_cpu_info(&logical_cores, &physical_cores, &base_freq, &l1, &l2, &l3);

                vga_write_color("CPU Information\n", COLOR_LIGHT_GREEN, COLOR_BLACK);
                vga_write_color(" Brand: ", COLOR_WHITE, COLOR_BLACK);
                vga_write(brand);
                vga_write_color("\n Physical Cores: ", COLOR_WHITE, COLOR_BLACK);
                print_num(physical_cores);
                vga_write_color("\n Logical Cores: ", COLOR_WHITE, COLOR_BLACK);
                print_num(logical_cores);
                if (base_freq)
                {
                    vga_write_color("\n Base Frequency: ", COLOR_WHITE, COLOR_BLACK);
                    print_num(base_freq);
                    vga_write(" MHz");
                }
                if (l1 || l2 || l3)
                {
                    vga_putc('\n');
                    vga_write_color(" L1 Cache: ", COLOR_WHITE, COLOR_BLACK);
                    print_num(l1);
                    vga_write(" KB\n");
                    vga_write_color(" L2 Cache: ", COLOR_WHITE, COLOR_BLACK);
                    print_num(l2);
                    vga_write(" KB\n");
                    vga_write_color(" L3 Cache: ", COLOR_WHITE, COLOR_BLACK);
                    print_num(l3);
                    vga_write(" KB");
                }
                break;
            }

            case 2:
                vga_write_color("Memory Information\n", COLOR_LIGHT_GREEN, COLOR_BLACK);
                vga_write_color(" Total: ", COLOR_WHITE, COLOR_BLACK);
                print_num(total_memory_mb);
                vga_write(" MB");
                break;

            case 3:
                vga_write_color("System\n", COLOR_LIGHT_GREEN, COLOR_BLACK);
                vga_write_color(" Manufacturer: ", COLOR_WHITE, COLOR_BLACK);
                vga_write(sys.manufacturer);
                vga_write_color("\n Product Name: ", COLOR_WHITE, COLOR_BLACK);
                vga_write(sys.product_name);
                vga_write_color("\n Version: ", COLOR_WHITE, COLOR_BLACK);
                vga_write(sys.version);
                vga_write_color("\n Serial: ", COLOR_WHITE, COLOR_BLACK);
                vga_write(sys.serial);
                vga_write_color("\n UUID: ", COLOR_WHITE, COLOR_BLACK);
                vga_write(sys.uuid);
                break;

            case 4:
                vga_write_color("Motherboard\n", COLOR_LIGHT_GREEN, COLOR_BLACK);
                vga_write_color(" Manufacturer: ", COLOR_WHITE, COLOR_BLACK);
                vga_write(board.manufacturer);
                vga_write_color("\n Product: ", COLOR_WHITE, COLOR_BLACK);
                vga_write(board.product);
                vga_write_color("\n Version: ", COLOR_WHITE, COLOR_BLACK);
                vga_write(board.version);
                vga_write_color("\n Serial: ", COLOR_WHITE, COLOR_BLACK);
                vga_write(board.serial);
                break;

            case 5:
                vga_write_color("BIOS Information\n", COLOR_LIGHT_GREEN, COLOR_BLACK);

                vga_write_color(" Vendor: ", COLOR_WHITE, COLOR_BLACK);
                vga_write(bios.vendor);

                vga_write_color("\n Version: ", COLOR_WHITE, COLOR_BLACK);
                vga_write(bios.version);

                vga_write_color("\n Date: ", COLOR_WHITE, COLOR_BLACK);
                vga_write(bios.date);

                vga_write_color("\n Runtime Size: ", COLOR_WHITE, COLOR_BLACK);
                print_num(bios.runtime_size);
                vga_write(" KB");

                vga_write_color("\n ROM Size: ", COLOR_WHITE, COLOR_BLACK);
                print_num(bios.rom_size);
                vga_write(" KB");

                vga_write_color("\n BIOS Revision: ", COLOR_WHITE, COLOR_BLACK);
                print_num(bios.bios_major);
                vga_putc('.');
                print_num(bios.bios_minor);

                vga_write_color("\n Firmware Revision: ", COLOR_WHITE, COLOR_BLACK);
                print_num(bios.fw_major);
                vga_putc('.');
                print_num(bios.fw_minor);
                break;
            }

            prev_tab = current;
        }

        int key = keyboard_getkey();
        if (key == KEY_LEFT && current > 0)
            current--;
        else if (key == KEY_RIGHT && current < total_tabs - 1)
            current++;
        else if (key == KEY_ESC)
            running = 0;
    }

    vga_clear();
    vga_enable_cursor(14, 15);
}

void about_os()
{
    vga_write_color("\n=== Operating System Information ===\n\n", COLOR_GREEN, COLOR_BLACK);
    vga_write_color(" Name: ", COLOR_WHITE, COLOR_BLACK);
    vga_write(OS_NAME);
    vga_write_color("\n Version: ", COLOR_WHITE, COLOR_BLACK);
    vga_write(OS_VERSION);
    vga_write_color("\n Kernel: ", COLOR_WHITE, COLOR_BLACK);
    vga_write(KERNEL);
    vga_write_color("\n Developer: ", COLOR_WHITE, COLOR_BLACK);
    vga_write(OS_DEVELOPER);
    vga_putc('\n');
}

void about_cpu()
{
    char brand[49];
    about_get_cpu_brand(brand);

    uint32_t logical_cores = 0, physical_cores = 0;
    uint32_t base_freq = 0, l1 = 0, l2 = 0, l3 = 0;
    about_get_cpu_info(&logical_cores, &physical_cores, &base_freq, &l1, &l2, &l3);

    vga_write_color("\n=== CPU Information ===\n\n", COLOR_GREEN, COLOR_BLACK);
    vga_write_color(" Brand: ", COLOR_WHITE, COLOR_BLACK);
    vga_write(brand);
    vga_write_color("\n Physical Cores: ", COLOR_WHITE, COLOR_BLACK);
    print_num(physical_cores);
    vga_write_color("\n Logical Cores: ", COLOR_WHITE, COLOR_BLACK);
    print_num(logical_cores);
    if (base_freq)
    {
        vga_write_color("\n Base Frequency: ", COLOR_WHITE, COLOR_BLACK);
        print_num(base_freq);
        vga_write(" MHz");
    }
    if (l1 || l2 || l3)
    {
        vga_putc('\n');
        vga_write_color(" L1 Cache: ", COLOR_WHITE, COLOR_BLACK);
        print_num(l1);
        vga_write(" KB\n");
        vga_write_color(" L2 Cache: ", COLOR_WHITE, COLOR_BLACK);
        print_num(l2);
        vga_write(" KB\n");
        vga_write_color(" L3 Cache: ", COLOR_WHITE, COLOR_BLACK);
        print_num(l3);
        vga_write(" KB");
    }
    vga_putc('\n');
}

void about_memory()
{
    vga_write_color("=== Memory Information ===\n\n", COLOR_GREEN, COLOR_BLACK);
    vga_write_color(" Total: ", COLOR_WHITE, COLOR_BLACK);
    print_num(total_memory_mb);
    vga_write(" MB\n");
}

void about_system()
{
    system_info_t sys = {"Unknown", "Unknown", "Unknown", "Unknown", "Unknown"};
    board_info_t board = {"Unknown", "Unknown", "Unknown", "Unknown"};
    bios_info_t bios = {"Unknown", "Unknown", "Unknown", 0, 0, 0, 0, 0, 0};
    about_get_smbios_info(&sys, &board, &bios);

    vga_write_color("\n=== System Information ===\n\n", COLOR_GREEN, COLOR_BLACK);
    vga_write_color(" Manufacturer: ", COLOR_WHITE, COLOR_BLACK);
    vga_write(sys.manufacturer);
    vga_write_color("\n Product Name: ", COLOR_WHITE, COLOR_BLACK);
    vga_write(sys.product_name);
    vga_write_color("\n Version: ", COLOR_WHITE, COLOR_BLACK);
    vga_write(sys.version);
    vga_write_color("\n Serial: ", COLOR_WHITE, COLOR_BLACK);
    vga_write(sys.serial);
    vga_write_color("\n UUID: ", COLOR_WHITE, COLOR_BLACK);
    vga_write(sys.uuid);
    vga_putc('\n');
}

void about_board()
{
    board_info_t board = {"Unknown", "Unknown", "Unknown", "Unknown"};
    system_info_t sys = {"Unknown", "Unknown", "Unknown", "Unknown", "Unknown"};
    bios_info_t bios = {"Unknown", "Unknown", "Unknown", 0, 0, 0, 0, 0, 0};
    about_get_smbios_info(&sys, &board, &bios);

    vga_write_color("\n=== Motherboard Information ===\n\n", COLOR_GREEN, COLOR_BLACK);
    vga_write_color(" Manufacturer: ", COLOR_WHITE, COLOR_BLACK);
    vga_write(board.manufacturer);
    vga_write_color("\n Product: ", COLOR_WHITE, COLOR_BLACK);
    vga_write(board.product);
    vga_write_color("\n Version: ", COLOR_WHITE, COLOR_BLACK);
    vga_write(board.version);
    vga_write_color("\n Serial: ", COLOR_WHITE, COLOR_BLACK);
    vga_write(board.serial);
    vga_putc('\n');
}

void about_init()
{
    register_command("about", about_command, "Show system information with tabs", "about", 0);
}
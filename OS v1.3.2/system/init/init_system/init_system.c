#include "init_system.h"
#include "../configs/drivers_config/drivers_config.h"
#include "../configs/plugins_config/plugins_config.h"
#include "../../kernel/drivers/VGA/vga.h"
#include "../../kernel/power_controller/poweroff/poweroff.h"
#include "../../kernel/power_controller/reboot/reboot.h"

static int compare_priority(driver_priority_t a, driver_priority_t b)
{
    if (a < b)
        return -1;
    else if (a > b)
        return 1;
    return 0;
}

static void sort_drivers_by_priority()
{
    for (int i = 1; i < drivers_count; i++)
    {
        DriverConfig key = drivers[i];
        int j = i - 1;

        while (j >= 0 && compare_priority(drivers[j].priority, key.priority) > 0)
        {
            drivers[j + 1] = drivers[j];
            j--;
        }
        drivers[j + 1] = key;
    }
}

static void init_drivers()
{
    sort_drivers_by_priority();

    vga_write_color("=== Initializing drivers ===\n", COLOR_GREEN, COLOR_BLACK);

    for (int i = 0; i < drivers_count; i++)
    {
        vga_write_color("=> Starting driver: ", COLOR_WHITE, COLOR_BLACK);
        vga_write(drivers[i].name);
        vga_putc('\n');

        if (drivers[i].init)
            drivers[i].init();

        vga_write_color("    Driver started: ", COLOR_LIGHT_GREEN, COLOR_BLACK);
        vga_write(drivers[i].name);
        vga_putc('\n');
    }

    vga_write_color("=== All drivers successfully initialized === \n\n", COLOR_GREEN, COLOR_BLACK);
}

static void stop_drivers()
{
    vga_write_color("=== Stopping drivers ===\n", COLOR_GREEN, COLOR_BLACK);

    for (int i = drivers_count - 1; i >= 0; i--)
    {
        if (drivers[i].stop)
        {
            vga_write_color("=> Stopping driver: ", COLOR_WHITE, COLOR_BLACK);
            vga_write(drivers[i].name);
            vga_putc('\n');

            drivers[i].stop();

            vga_write_color("   Driver stopped: ", COLOR_LIGHT_GREEN, COLOR_BLACK);
            vga_write(drivers[i].name);
            vga_putc('\n');
        }
        else
        {
            vga_write_color("   Driver has no stop function: ", COLOR_LIGHT_RED, COLOR_BLACK);
            vga_write(drivers[i].name);
            vga_putc('\n');
        }
    }

    vga_write_color("=== All drivers stopped ===\n\n", COLOR_GREEN, COLOR_BLACK);
}

static void load_plugins()
{
    vga_write_color("=== Initializing plug-ins ===\n", COLOR_GREEN, COLOR_BLACK);

    vga_write_color("=== Loading command plug-ins ===\n", COLOR_LIGHT_CYAN, COLOR_BLACK);
    for (int i = 0; i < plugins_count; i++)
    {
        if (plugins[i].type == PLUGIN_TYPE_COMMAND && plugins[i].init)
        {
            vga_write_color("=> Loading command plug-in: ", COLOR_WHITE, COLOR_BLACK);
            vga_write(plugins[i].name);
            vga_putc('\n');
            plugins[i].init();
            vga_write_color("   Command plug-in loaded: ", COLOR_LIGHT_GREEN, COLOR_BLACK);
            vga_write(plugins[i].name);
            vga_putc('\n');
        }
    }

    vga_write_color("\n=== Starting plug-ins ===\n", COLOR_LIGHT_CYAN, COLOR_BLACK);
    for (int i = 0; i < plugins_count; i++)
    {
        if (plugins[i].type == PLUGIN_TYPE_MODULE && plugins[i].start)
        {
            vga_write_color("=> Starting plug-in: ", COLOR_WHITE, COLOR_BLACK);
            vga_write(plugins[i].name);
            vga_putc('\n');
            plugins[i].start();
            vga_write_color("   Plug-in started: ", COLOR_LIGHT_GREEN, COLOR_BLACK);
            vga_write(plugins[i].name);
            vga_putc('\n');
        }
        else if (plugins[i].type == PLUGIN_TYPE_MODULE)
        {
            vga_write_color("   Plug-in (no commands): ", COLOR_LIGHT_GRAY, COLOR_BLACK);
            vga_write(plugins[i].name);
            vga_putc('\n');
        }
    }

    vga_write_color("=== All plug-ins successfully initialized ===\n\n", COLOR_GREEN, COLOR_BLACK);
}

static void stop_plugins()
{
    vga_write_color("=== Stopping plug-ins ===\n", COLOR_GREEN, COLOR_BLACK);

    for (int i = plugins_count - 1; i >= 0; i--)
    {
        if (plugins[i].type == PLUGIN_TYPE_MODULE && plugins[i].stop)
        {
            vga_write_color("=> Stopping plug-in: ", COLOR_WHITE, COLOR_BLACK);
            vga_write(plugins[i].name);
            vga_putc('\n');

            plugins[i].stop();

            vga_write_color("   Plug-in stopped: ", COLOR_LIGHT_GREEN, COLOR_BLACK);
            vga_write(plugins[i].name);
            vga_putc('\n');
        }
        else if (plugins[i].type == PLUGIN_TYPE_MODULE && !plugins[i].stop)
        {
            vga_write_color("   Plug-in has no stop function: ", COLOR_LIGHT_RED, COLOR_BLACK);
            vga_write(plugins[i].name);
            vga_putc('\n');
        }
    }

    vga_write_color("=== All plug-ins stopped ===\n\n", COLOR_GREEN, COLOR_BLACK);
}

void system_init()
{
    vga_write_color("=== Initializing system ===\n\n", COLOR_LIGHT_GREEN, COLOR_BLACK);

    init_drivers();
    load_plugins();

    vga_write_color("=== System initialization complete ===\n\n", COLOR_LIGHT_GREEN, COLOR_BLACK);
}

void system_poweroff()
{
    vga_clear();
    vga_write_color("=== Powering off system ===\n\n", COLOR_GREEN, COLOR_BLACK);

    stop_plugins();
    stop_drivers();

    power_poweroff();
}

void system_reboot()
{
    vga_clear();
    vga_write_color("=== Rebooting system ===\n\n", COLOR_GREEN, COLOR_BLACK);

    stop_plugins();
    stop_drivers();

    power_reboot();
}
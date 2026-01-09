#include "../system/kernel/drivers/VGA/vga.h"
#include "../system/init/configs/drivers_config/drivers_config.h"
#include "../system/init/configs/plugins_config/plugins_config.h"
#include "../system/kernel/drivers/keyboard/keyboard.h"
#include "../libraries/string/string.h"
#include "shell/command_interpreter/command_interpreter.h"

#define DRIVERS_PER_PAGE 3
#define PLUGINS_PER_PAGE 3
#define DRIVERS_ITEM_HEIGHT 4
#define PLUGINS_ITEM_HEIGHT 4
#define DRIVERS_START_ROW 4
#define PLUGINS_START_ROW 4

static int drivers_total_pages = 0;
static int drivers_current_page = 0;
static int drivers_is_active = 0;

static int plugins_total_pages = 0;
static int plugins_current_page = 0;
static int plugins_is_active = 0;

static void installed_drivers_command_impl();
static void installed_plugins_command_impl();

static void sort_drivers_alphabetically()
{
    for (int i = 0; i < drivers_count; i++)
    {
        for (int j = i + 1; j < drivers_count; j++)
        {
            int name_cmp = strcasecmp(drivers[i].name, drivers[j].name);

            if (name_cmp > 0 ||
                (name_cmp == 0 && drivers[i].priority > drivers[j].priority))
            {
                DriverConfig temp = drivers[i];
                drivers[i] = drivers[j];
                drivers[j] = temp;
            }
        }
    }
}

static const char *get_priority_name(driver_priority_t priority)
{
    switch (priority)
    {
    case DRIVER_PRIORITY_HIGH:
        return "High";
    case DRIVER_PRIORITY_NORMAL:
        return "Normal";
    case DRIVER_PRIORITY_LOW:
        return "Low";
    default:
        return "Unknown";
    }
}

static void draw_drivers_static_elements()
{
    vga_disable_cursor();

    vga_set_row_col(0, 0);
    const char *top_frame = "╔══════════════════════════════════════════════╗";
    const char *title_frame = "║             Installed Drivers List           ║";
    const char *bottom_frame = "╚══════════════════════════════════════════════╝";

    vga_write_color_centered(top_frame, COLOR_LIGHT_GRAY, COLOR_BLACK);
    vga_write_color_centered(title_frame, COLOR_LIGHT_GRAY, COLOR_BLACK);
    vga_write_color_centered(bottom_frame, COLOR_LIGHT_GRAY, COLOR_BLACK);

    vga_set_row_col(DRIVERS_START_ROW + (DRIVERS_PER_PAGE * DRIVERS_ITEM_HEIGHT), 0);
    const char *separator = "────────────────────────────────────────────────────────────────────────────────";
    vga_write_color_centered(separator, COLOR_DARK_GRAY, COLOR_BLACK);
}

static void draw_drivers_dynamic_elements()
{
    int drivers_area_end = DRIVERS_START_ROW + (DRIVERS_PER_PAGE * DRIVERS_ITEM_HEIGHT) - 1;
    vga_clear_area(DRIVERS_START_ROW, drivers_area_end);

    vga_set_row_col(DRIVERS_START_ROW, 0);

    int start = drivers_current_page * DRIVERS_PER_PAGE;
    int end = start + DRIVERS_PER_PAGE;
    if (end > drivers_count)
        end = drivers_count;

    if (drivers_count == 0)
    {
        vga_write_color("No drivers installed.\n", COLOR_LIGHT_RED, COLOR_BLACK);
    }
    else
    {
        for (int i = start; i < end; i++)
        {
            char buffer[128];
            str_format(buffer, sizeof(buffer), "[%d] %s", i + 1, drivers[i].name);
            vga_write_color(buffer, COLOR_LIGHT_CYAN, COLOR_BLACK);
            vga_putc('\n');

            vga_write_color("  └ Description: ", COLOR_LIGHT_GRAY, COLOR_BLACK);
            if (drivers[i].description)
                vga_write_color(drivers[i].description, COLOR_WHITE, COLOR_BLACK);
            else
                vga_write_color("(No description)", COLOR_DARK_GRAY, COLOR_BLACK);
            vga_putc('\n');

            vga_write_color("  └ Priority: ", COLOR_LIGHT_GRAY, COLOR_BLACK);
            vga_write_color(get_priority_name(drivers[i].priority), COLOR_YELLOW, COLOR_BLACK);
            vga_putc('\n');
            vga_putc('\n');
        }
    }

    int footer_row = DRIVERS_START_ROW + (DRIVERS_PER_PAGE * DRIVERS_ITEM_HEIGHT) + 1;
    vga_clear_area(footer_row, footer_row);

    vga_set_row_col(footer_row, 0);
    char footer[64];
    str_format(footer, sizeof(footer), "[Page %d/%d]  ←/→  Navigate   ESC  Exit", drivers_current_page + 1, drivers_total_pages);
    vga_write_color_centered(footer, COLOR_LIGHT_GREEN, COLOR_BLACK);
}

static void installed_drivers_command_impl()
{
    vga_disable_cursor();
    drivers_is_active = 1;
    sort_drivers_alphabetically();

    drivers_total_pages = (drivers_count + DRIVERS_PER_PAGE - 1) / DRIVERS_PER_PAGE;
    drivers_current_page = 0;
    int running = 1;

    vga_clear();

    draw_drivers_static_elements();
    draw_drivers_dynamic_elements();

    while (running && drivers_is_active)
    {
        int key = keyboard_getkey();
        int old_page = drivers_current_page;

        if (key == KEY_RIGHT)
        {
            if (drivers_current_page < drivers_total_pages - 1)
                drivers_current_page++;
        }
        else if (key == KEY_LEFT)
        {
            if (drivers_current_page > 0)
                drivers_current_page--;
        }
        else if (key == KEY_ESC)
        {
            running = 0;
        }
        else
        {
            continue;
        }

        if (drivers_current_page != old_page)
        {
            draw_drivers_dynamic_elements();
        }
    }

    drivers_is_active = 0;
    vga_clear();
    vga_enable_cursor(14, 15);
}

static void sort_plugins_alphabetically()
{
    for (int i = 0; i < plugins_count; i++)
    {
        for (int j = i + 1; j < plugins_count; j++)
        {
            int name_cmp = strcasecmp(plugins[i].name, plugins[j].name);
            if (name_cmp > 0)
            {
                PluginConfig temp = plugins[i];
                plugins[i] = plugins[j];
                plugins[j] = temp;
            }
        }
    }
}

static const char *get_plugin_type_name(PluginType type)
{
    switch (type)
    {
    case PLUGIN_TYPE_MODULE:
        return "Module";
    case PLUGIN_TYPE_COMMAND:
        return "Command";
    default:
        return "Unknown";
    }
}

static void draw_plugins_static_elements()
{
    vga_disable_cursor();

    vga_set_row_col(0, 0);
    const char *top_frame = "╔══════════════════════════════════════════════╗";
    const char *title_frame = "║             Installed Plugins List           ║";
    const char *bottom_frame = "╚══════════════════════════════════════════════╝";

    vga_write_color_centered(top_frame, COLOR_LIGHT_GRAY, COLOR_BLACK);
    vga_write_color_centered(title_frame, COLOR_LIGHT_GRAY, COLOR_BLACK);
    vga_write_color_centered(bottom_frame, COLOR_LIGHT_GRAY, COLOR_BLACK);

    vga_set_row_col(PLUGINS_START_ROW + (PLUGINS_PER_PAGE * PLUGINS_ITEM_HEIGHT), 0);
    const char *separator = "────────────────────────────────────────────────────────────────────────────────";
    vga_write_color_centered(separator, COLOR_DARK_GRAY, COLOR_BLACK);
}

static void draw_plugins_dynamic_elements()
{
    int plugins_area_end = PLUGINS_START_ROW + (PLUGINS_PER_PAGE * PLUGINS_ITEM_HEIGHT) - 1;
    vga_clear_area(PLUGINS_START_ROW, plugins_area_end);

    vga_set_row_col(PLUGINS_START_ROW, 0);

    int start = plugins_current_page * PLUGINS_PER_PAGE;
    int end = start + PLUGINS_PER_PAGE;
    if (end > plugins_count)
        end = plugins_count;

    if (plugins_count == 0)
    {
        vga_write_color("No plugins installed.\n", COLOR_LIGHT_RED, COLOR_BLACK);
    }
    else
    {
        for (int i = start; i < end; i++)
        {
            char buffer[128];
            str_format(buffer, sizeof(buffer), "[%d] %s", i + 1, plugins[i].name);
            vga_write_color(buffer, COLOR_LIGHT_CYAN, COLOR_BLACK);
            vga_putc('\n');

            vga_write_color("  ├ Type: ", COLOR_LIGHT_GRAY, COLOR_BLACK);
            vga_write_color(get_plugin_type_name(plugins[i].type), COLOR_YELLOW, COLOR_BLACK);
            vga_putc('\n');

            vga_write_color("  └ Description: ", COLOR_LIGHT_GRAY, COLOR_BLACK);
            if (plugins[i].description)
                vga_write_color(plugins[i].description, COLOR_WHITE, COLOR_BLACK);
            else
                vga_write_color("(No description)", COLOR_DARK_GRAY, COLOR_BLACK);
            vga_putc('\n');
            vga_putc('\n');
        }
    }

    int footer_row = PLUGINS_START_ROW + (PLUGINS_PER_PAGE * PLUGINS_ITEM_HEIGHT) + 1;
    vga_clear_area(footer_row, footer_row);

    vga_set_row_col(footer_row, 0);
    char footer[64];
    str_format(footer, sizeof(footer), "[Page %d/%d]  ←/→  Navigate   ESC  Exit", plugins_current_page + 1, plugins_total_pages);
    vga_write_color_centered(footer, COLOR_LIGHT_GREEN, COLOR_BLACK);
}

static void installed_plugins_command_impl()
{
    vga_disable_cursor();
    plugins_is_active = 1;
    sort_plugins_alphabetically();

    plugins_total_pages = (plugins_count + PLUGINS_PER_PAGE - 1) / PLUGINS_PER_PAGE;
    plugins_current_page = 0;
    int running = 1;

    vga_clear();

    draw_plugins_static_elements();
    draw_plugins_dynamic_elements();

    while (running && plugins_is_active)
    {
        int key = keyboard_getkey();
        int old_page = plugins_current_page;

        if (key == KEY_RIGHT)
        {
            if (plugins_current_page < plugins_total_pages - 1)
                plugins_current_page++;
        }
        else if (key == KEY_LEFT)
        {
            if (plugins_current_page > 0)
                plugins_current_page--;
        }
        else if (key == KEY_ESC)
        {
            running = 0;
        }
        else
        {
            continue;
        }

        if (plugins_current_page != old_page)
        {
            draw_plugins_dynamic_elements();
        }
    }

    plugins_is_active = 0;
    vga_clear();
    vga_enable_cursor(14, 15);
}

void installed_command(const char *args)
{
    if (args == NULL || strlen(args) == 0)
    {
        vga_write_color("Usage: installed <argument>\n\n", COLOR_LIGHT_GRAY, COLOR_BLACK);
        vga_write_color("Available arguments:\n", COLOR_LIGHT_CYAN, COLOR_BLACK);
        vga_write_color("  drivers - Show installed drivers\n", COLOR_WHITE, COLOR_BLACK);
        vga_write_color("  plugins - Show installed plugins\n", COLOR_WHITE, COLOR_BLACK);
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

    if (strcmp(arg, "drivers") == 0)
    {
        installed_drivers_command_impl();
    }
    else if (strcmp(arg, "plugins") == 0)
    {
        installed_plugins_command_impl();
    }
    else
    {
        vga_write_color("Error: Unknown argument '", COLOR_RED, COLOR_BLACK);
        vga_write_color(arg, COLOR_LIGHT_RED, COLOR_BLACK);
        vga_write_color("'\n", COLOR_RED, COLOR_BLACK);
        vga_write_color("Use 'installed' without arguments to see available options.\n", COLOR_LIGHT_GRAY, COLOR_BLACK);
    }
}

void installed_init()
{
    register_command("installed", installed_command, "Show installed components", "installed [drivers|plugins]", 1);
}
#include "notify.h"
#include "../../../system/kernel/drivers/VGA/vga.h"
#include "../../../libraries/string/string.h"
#include <stdarg.h>

static void notify_set_color(notify_type_t type) 
{
    switch(type) {
        case NOTIFY_STANDARD_1:
            vga_setcolor(COLOR_LIGHT_GRAY, COLOR_BLACK);
            break;
        case NOTIFY_STANDARD_2:
            vga_setcolor(COLOR_WHITE, COLOR_BLACK);
            break;
        case NOTIFY_SUCCESS:
            vga_setcolor(COLOR_GREEN, COLOR_BLACK);
            break;
        case NOTIFY_INFO_1:
            vga_setcolor(COLOR_CYAN, COLOR_BLACK);
            break;
        case NOTIFY_INFO_2:
            vga_setcolor(COLOR_LIGHT_CYAN, COLOR_BLACK);
            break;
        case NOTIFY_WARNING:
            vga_setcolor(COLOR_YELLOW, COLOR_BLACK);
            break;
        case NOTIFY_ERROR:
            vga_setcolor(COLOR_RED, COLOR_BLACK);
            break;
        case NOTIFY_ERROR_DETAIL:
            vga_setcolor(COLOR_LIGHT_RED, COLOR_BLACK);
            break;
        case NOTIFY_SYSTEM:
            vga_setcolor(COLOR_MAGENTA, COLOR_BLACK);
            break;
        default:
            vga_setcolor(COLOR_LIGHT_GRAY, COLOR_BLACK);
    }
}

void notify(notify_type_t type, const char *message) 
{
    notify_set_color(type);
    vga_write(message);
    
    vga_setcolor(COLOR_LIGHT_GRAY, COLOR_BLACK);
}

void notify_fmt(notify_type_t type, const char *format, ...) 
{
    char buffer[256];
    va_list args;
    
    notify_set_color(type);
    
    va_start(args, format);
    str_format(buffer, sizeof(buffer), format, args);
    vga_write(buffer);
    va_end(args);
}
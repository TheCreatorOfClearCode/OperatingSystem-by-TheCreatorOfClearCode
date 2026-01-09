#include "shell/command_interpreter/command_interpreter.h"
#include "../system/kernel/drivers/RTC/rtc.h"
#include "../system/kernel/drivers/VGA/vga.h"
#include "shell/notify/notify.h"
#include "../libraries/string/string.h"

static void datetime_command()
{
    uint8_t h, m, s, d, mon;
    uint16_t y;
    rtc_get_datetime(&h, &m, &s, &d, &mon, &y);

    char buf[64];
    str_format(buf, sizeof(buf), "Date: %02d.%02d.%04d  Time: %02d:%02d:%02d\n", d, mon, y, h, m, s);
    vga_write(buf);
}

void datetime_init()
{
    register_command("date&time", datetime_command, "Shows the date and time", "date&time", 0);
}
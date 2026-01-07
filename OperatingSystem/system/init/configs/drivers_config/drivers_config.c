#include "drivers_config.h"
#include "../../../kernel/drivers/IO/io.h"
#include "../../../kernel/drivers/VGA/vga.h"
#include "../../../kernel/drivers/keyboard/keyboard.h"
#include "../../../kernel/drivers/RTC/rtc.h"

DriverConfig drivers[] = {
    {"VGA Driver", "Handles video output via VGA interface", vga_init, vga_stop, DRIVER_PRIORITY_HIGH},
    {"Input/Output Driver", "Manages low-level port I/O operations", io_init, io_stop, DRIVER_PRIORITY_HIGH},
    {"Keyboard Driver", "Processes keyboard input events", keyboard_init, keyboard_stop, DRIVER_PRIORITY_HIGH},
    {"Real Time Clock Driver", "Provides time and date from CMOS", rtc_init, rtc_stop, DRIVER_PRIORITY_LOW}
};

int drivers_count = sizeof(drivers) / sizeof(drivers[0]);
#include "reboot.h"
#include "../../drivers/IO/io.h"

void power_reboot() 
{
    outb(0x64, 0xFE);
}
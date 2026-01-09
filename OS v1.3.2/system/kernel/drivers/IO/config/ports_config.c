#include "ports_config.h"

const io_port_t io_ports[] = {
    {"VGA", 0x3D4},
    {"PS/2", 0x64},
    {"COM1", 0x3F8},
    {"COM2", 0x2F8},
    {"PIT", 0x40},
    {"CMOS", 0x70},
    {"PIC", 0x20},
    {"PCI", 0xCF8}
};

const int io_port_count = sizeof(io_ports) / sizeof(io_ports[0]);
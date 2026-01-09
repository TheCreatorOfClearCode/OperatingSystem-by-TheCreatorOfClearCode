#include "io.h"
#include "config/ports_config.h"
#include "../VGA/vga.h"
#include "../../../../libraries/string/string.h"

void outb(uint16_t port, uint8_t value)
{
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void outw(uint16_t port, uint16_t value)
{
    asm volatile("outw %0, %1" : : "a"(value), "Nd"(port));
}

uint16_t inw(uint16_t port)
{
    uint16_t ret;
    asm volatile("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void outl(uint16_t port, uint32_t value)
{
    asm volatile("outl %0, %1" : : "a"(value), "Nd"(port));
}

uint32_t inl(uint16_t port)
{
    uint32_t ret;
    asm volatile("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void io_wait()
{
    asm volatile("outb %%al, $0x80" : : "a"(0));
}

typedef enum
{
    PORT_READABLE,
    PORT_WRITEONLY,
    PORT_VOLATILE,
    PORT_STATEFUL,
    PORT_UNKNOWN
} port_behavior_t;

typedef struct
{
    port_behavior_t behavior;
    const char *type_name;
    uint16_t related_port;
} port_analysis_t;

static port_analysis_t analyze_port_behavior(uint16_t port)
{
    port_analysis_t result = {PORT_UNKNOWN, "Unknown", 0};

    uint8_t reads[8];
    int all_ff = 1;
    int all_same = 1;
    int stable_pattern = 1;

    for (int i = 0; i < 8; i++)
    {
        reads[i] = inb(port);
        io_wait();

        if (reads[i] != 0xFF)
            all_ff = 0;
        if (i > 0 && reads[i] != reads[0])
            all_same = 0;
        if (i > 1 && reads[i] != reads[i - 1])
            stable_pattern = 0;
    }

    uint8_t reaction_to_write = 0;
    if (!all_ff)
    {
        uint8_t original = reads[0];

        outb(port, ~original);
        io_wait();
        uint8_t after_invert = inb(port);
        io_wait();

        outb(port, 0x55);
        io_wait();
        uint8_t after_pattern1 = inb(port);
        io_wait();

        outb(port, 0xAA);
        io_wait();
        uint8_t after_pattern2 = inb(port);
        io_wait();

        outb(port, original);
        io_wait();

        if (after_invert != original && after_invert != 0xFF)
            reaction_to_write |= 1;
        if (after_pattern1 != 0xFF)
            reaction_to_write |= 2;
        if (after_pattern2 != 0xFF)
            reaction_to_write |= 4;
    }

    if (all_ff)
    {
        outb(port, 0x00);
        io_wait();
        outb(port, 0xFF);
        io_wait();
        result.behavior = PORT_WRITEONLY;
        result.type_name = "Write-only";
        return result;
    }

    if (all_same && reaction_to_write == 0)
    {
        result.behavior = PORT_READABLE;
        result.type_name = "Readable";
        return result;
    }

    if (!stable_pattern && reaction_to_write == 0)
    {
        result.behavior = PORT_VOLATILE;
        result.type_name = "Volatile";
        return result;
    }

    if (reaction_to_write > 0)
    {
        result.behavior = PORT_STATEFUL;
        result.type_name = "Stateful";

        for (int offset = 1; offset <= 4; offset++)
        {
            if (port + offset < 0xFFFF)
            {
                uint8_t neighbor = inb(port + offset);
                io_wait();
                if (neighbor != 0xFF)
                {
                    uint8_t orig_neighbor = neighbor;
                    outb(port, 0x55);
                    io_wait();
                    uint8_t new_neighbor = inb(port + offset);
                    io_wait();
                    outb(port, reads[0]);
                    io_wait();

                    if (new_neighbor != orig_neighbor)
                    {
                        result.related_port = port + offset;
                        break;
                    }
                }
            }

            if (port - offset > 0)
            {
                uint8_t neighbor = inb(port - offset);
                io_wait();
                if (neighbor != 0xFF)
                {
                    uint8_t orig_neighbor = neighbor;
                    outb(port, 0x55);
                    io_wait();
                    uint8_t new_neighbor = inb(port - offset);
                    io_wait();
                    outb(port, reads[0]);
                    io_wait();

                    if (new_neighbor != orig_neighbor)
                    {
                        result.related_port = port - offset;
                        break;
                    }
                }
            }
        }

        return result;
    }

    result.behavior = PORT_READABLE;
    result.type_name = "Readable (auto)";
    return result;
}

static int test_port_by_behavior(uint16_t port, port_behavior_t behavior, uint16_t related_port)
{
    switch (behavior)
    {
    case PORT_READABLE:
        return inb(port) != 0xFF;

    case PORT_WRITEONLY:
        outb(port, 0x00);
        io_wait();
        outb(port, 0xFF);
        io_wait();
        return 1;

    case PORT_VOLATILE:
        uint8_t val1 = inb(port);
        io_wait();
        uint8_t val2 = inb(port);
        io_wait();
        uint8_t val3 = inb(port);
        io_wait();
        return (val1 != val2) || (val2 != val3) || (val1 != val3);

    case PORT_STATEFUL:
        uint8_t original = inb(port);
        io_wait();

        outb(port, 0x55);
        io_wait();
        uint8_t after_write1 = inb(port);
        io_wait();

        outb(port, 0xAA);
        io_wait();
        uint8_t after_write2 = inb(port);
        io_wait();

        outb(port, original);
        io_wait();

        int reacted = (after_write1 != 0xFF) || (after_write2 != 0xFF);

        if (related_port != 0)
        {
            uint8_t related_before = inb(related_port);
            io_wait();
            outb(port, 0x55);
            io_wait();
            uint8_t related_after = inb(related_port);
            io_wait();
            outb(port, original);
            io_wait();

            reacted = reacted || (related_before != related_after);
        }

        return reacted;

    case PORT_UNKNOWN:
    default:
        uint8_t safe_read = inb(port);
        io_wait();
        if (safe_read != 0xFF)
            return 1;

        outb(port, 0x00);
        io_wait();
        return 1;
    }
}

static void print_status(const char *status, uint8_t color, const char *type_name, uint16_t related_port)
{
    vga_write_color(" (", COLOR_LIGHT_GRAY, COLOR_BLACK);
    vga_write_color(status, color, COLOR_BLACK);
    vga_write_color(" - ", COLOR_LIGHT_GRAY, COLOR_BLACK);
    vga_write_color(type_name, COLOR_YELLOW, COLOR_BLACK);

    if (related_port != 0)
    {
        char related_buf[24];
        str_format(related_buf, sizeof(related_buf), " related to 0x%X", related_port);
        vga_write_color(related_buf, COLOR_LIGHT_BLUE, COLOR_BLACK);
    }

    vga_write_color(")\n", COLOR_LIGHT_GRAY, COLOR_BLACK);
}

static void print_spaces(int count)
{
    for (int i = 0; i < count; i++)
        vga_write_color(" ", COLOR_BLACK, COLOR_BLACK);
}

static void check_port(const io_port_t *p, int max_name_len)
{
    vga_write_color("[", COLOR_LIGHT_GRAY, COLOR_BLACK);
    vga_write_color(p->name, COLOR_CYAN, COLOR_BLACK);
    vga_write_color("]", COLOR_LIGHT_GRAY, COLOR_BLACK);

    int pad = (max_name_len - strlen(p->name)) + 2;
    print_spaces(pad);

    char buf[32];
    str_format(buf, sizeof(buf), "Port 0x%X", p->port);
    vga_write_color(buf, COLOR_WHITE, COLOR_BLACK);

    port_analysis_t analysis = analyze_port_behavior(p->port);

    int result = test_port_by_behavior(p->port, analysis.behavior, analysis.related_port);

    if (result)
    {
        print_status("OK", COLOR_GREEN, analysis.type_name, analysis.related_port);
    }
    else
    {
        print_status("No response", COLOR_RED, analysis.type_name, analysis.related_port);
    }
}

void io_init()
{
    vga_write_color("=== Initializing Input/Output subsystem ===\n", COLOR_GREEN, COLOR_BLACK);

    int max_name_len = 0;
    for (int i = 0; i < io_port_count; i++)
    {
        int len = strlen(io_ports[i].name);
        if (len > max_name_len)
            max_name_len = len;
    }

    for (int i = 0; i < io_port_count; i++)
        check_port(&io_ports[i], max_name_len);

    vga_write_color("=== Input/Output subsystem ready ===\n", COLOR_GREEN, COLOR_BLACK);
}

void io_stop()
{
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);

    outb(0x43, 0x30);
    outb(0x40, 0);
    outb(0x40, 0);

    io_wait();
}
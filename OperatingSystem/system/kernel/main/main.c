#include <stdint.h>
#include "../drivers/VGA/vga.h"
#include "../../init/init_system/init_system.h"
#include "multiboot.h"
#include "../../../libraries/string/string.h"
#include "../../about_system/about_system.h"

const char *KERNEL = "Kernel v1.3.1 (x86) by TheCreatorOfClearCode";
uint32_t total_memory_mb = 0;

static void print_status_ok()
{
    vga_write_color(" (", COLOR_LIGHT_GRAY, COLOR_BLACK);
    vga_write_color("OK", COLOR_GREEN, COLOR_BLACK);
    vga_write_color(")\n", COLOR_LIGHT_GRAY, COLOR_BLACK);
}

static void print_status_fail()
{
    vga_write_color(" (", COLOR_LIGHT_GRAY, COLOR_BLACK);
    vga_write_color("FAIL", COLOR_RED, COLOR_BLACK);
    vga_write_color(")\n", COLOR_LIGHT_GRAY, COLOR_BLACK);
}

static const char *get_cpu_mode()
{
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    return (cr0 & 0x1) ? "Protected" : "Real";
}

static void check_cpu_mode()
{
    uint32_t cr0;
    vga_write_color("=> Checking CPU operating mode...\n", COLOR_WHITE, COLOR_BLACK);

    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    char cr0_str[32];
    str_format(cr0_str, sizeof(cr0_str), "0x%08X", cr0);
    vga_write_color("[CPU MODE] Step 1: CR0 register value = ", COLOR_LIGHT_GRAY, COLOR_BLACK);
    vga_write_color(cr0_str, COLOR_LIGHT_GRAY, COLOR_BLACK);
    vga_putc('\n');

    int pe_bit = cr0 & 0x1;
    char buf[64];
    str_format(buf, sizeof(buf), "[CPU MODE] Step 2: Protection Enable (PE) bit = %d\n", pe_bit);
    vga_write_color(buf, COLOR_LIGHT_GRAY, COLOR_BLACK);

    if (pe_bit)
    {
        vga_write_color("[CPU MODE] Step 3: CPU is running in Protected Mode.\n", COLOR_GREEN, COLOR_BLACK);
        vga_write_color(" Mode check: ", COLOR_WHITE, COLOR_BLACK);
        print_status_ok();
        vga_putc('\n');
    }
    else
    {
        vga_write_color("[CPU MODE] Step 3: CPU is running in Real Mode!\n", COLOR_RED, COLOR_BLACK);
        print_status_fail();
        vga_putc('\n');

        vga_write_color("ERROR: CPU is in Real Mode!\n", COLOR_RED, COLOR_BLACK);
        vga_write_color("This kernel requires Protected Mode (PE=1 in CR0).", COLOR_LIGHT_GRAY, COLOR_BLACK);

        for (;;)
            asm volatile("hlt");
    }
}

static const char *get_architecture()
{
#if defined(__x86_64__) || defined(_M_X64)
    return "x86_64";
#else
    return "x86";
#endif
}

static void print_kernel_info()
{
    vga_write_color("Kernel info:\n", COLOR_WHITE, COLOR_BLACK);

    vga_write_color(" Version: ", COLOR_LIGHT_GRAY, COLOR_BLACK);
    vga_write_color(KERNEL, COLOR_LIGHT_GRAY, COLOR_BLACK);
    vga_putc('\n');

    vga_write_color(" Architecture: ", COLOR_LIGHT_GRAY, COLOR_BLACK);
    vga_write_color(get_architecture(), COLOR_LIGHT_GRAY, COLOR_BLACK);
    vga_putc('\n');

    vga_write_color(" Mode: ", COLOR_LIGHT_GRAY, COLOR_BLACK);
    vga_write_color(get_cpu_mode(), COLOR_LIGHT_GRAY, COLOR_BLACK);
    vga_putc('\n');
    vga_putc('\n');
}

static void check_multiboot_signature(uint32_t magic)
{
    vga_write_color("=> Checking Multiboot signature... ", COLOR_WHITE, COLOR_BLACK);

    if (magic != 0x2BADB002)
    {
        print_status_fail();
        vga_write_color("Invalid Multiboot signature (expected 0x2BADB002)\n", COLOR_RED, COLOR_BLACK);

        for (;;)
            __asm__ volatile("hlt");
    }
    else
    {
        print_status_ok();
    }
}

static int check_smbios_presence()
{
    uint8_t *base = (uint8_t *)0xF0000;
    vga_write_color("=> Checking SMBIOS access...\n", COLOR_WHITE, COLOR_BLACK);

    for (uint32_t off = 0; off < 0x10000; off += 16)
    {
        uint8_t *p = base + off;

        if (p[0] == '_' && p[1] == 'S' && p[2] == 'M' && p[3] == '_')
        {
            char addr_str[32];
            str_format(addr_str, sizeof(addr_str), "0x%08X", (uint32_t)p);
            vga_write_color("[SMBIOS] Signature found at: ", COLOR_LIGHT_GRAY, COLOR_BLACK);
            vga_write_color(addr_str, COLOR_LIGHT_GRAY, COLOR_BLACK);
            vga_putc('\n');

            uint8_t len = p[5];
            uint8_t sum = 0;
            for (uint8_t i = 0; i < len; i++)
                sum += p[i];

            vga_write_color("[SMBIOS] Checksum: ", COLOR_LIGHT_GRAY, COLOR_BLACK);
            if (sum == 0)
                vga_write_color("OK\n", COLOR_GREEN, COLOR_BLACK);
            else
                vga_write_color("FAIL\n", COLOR_RED, COLOR_BLACK);

            if (sum != 0)
                continue;

            system_info_t sys = {0};
            board_info_t board = {0};
            bios_info_t bios = {0};
            about_get_smbios_info(&sys, &board, &bios);

            if (bios.vendor && bios.vendor[0] != '\0' && bios.vendor != (const char *)"N/A")
            {
                vga_write_color("[SMBIOS] Vendor: ", COLOR_LIGHT_GRAY, COLOR_BLACK);
                vga_write_color(bios.vendor, COLOR_LIGHT_GRAY, COLOR_BLACK);
                vga_putc('\n');
            }
            else
            {
                vga_write_color("[SMBIOS] Vendor: Unknown\n", COLOR_LIGHT_GRAY, COLOR_BLACK);
            }

            uint8_t major = p[6];
            uint8_t minor = p[7];
            char ver_str[16];
            str_format(ver_str, sizeof(ver_str), "%d.%d", major, minor);
            vga_write_color("[SMBIOS] Version: ", COLOR_LIGHT_GRAY, COLOR_BLACK);
            vga_write_color(ver_str, COLOR_LIGHT_GRAY, COLOR_BLACK);
            vga_putc('\n');

            uint32_t table_addr = *(uint32_t *)(p + 0x18);

            char table_str[32];
            str_format(table_str, sizeof(table_str), "0x%08X", table_addr);
            vga_write_color("[SMBIOS] Table address: ", COLOR_LIGHT_GRAY, COLOR_BLACK);
            vga_write_color(table_str, COLOR_LIGHT_GRAY, COLOR_BLACK);
            vga_putc('\n');

            print_status_ok();
            vga_write_color("SMBIOS is available.\n\n", COLOR_GREEN, COLOR_BLACK);
            return 1;
        }
    }

    print_status_fail();
    vga_write_color("SMBIOS not found or checksum invalid.\n\n", COLOR_RED, COLOR_BLACK);
    return 0;
}

static void check_cpuid_support()
{
    vga_write_color("=> Checking CPUID instruction support...\n", COLOR_WHITE, COLOR_BLACK);
    vga_write_color("[CPUID] Initializing EFLAGS test procedure.\n", COLOR_LIGHT_GRAY, COLOR_BLACK);

    uint32_t eflags_before, eflags_after;
    char buf[128];

    asm volatile("pushfl\n\t" "popl %0" : "=r"(eflags_before));

    str_format(buf, sizeof(buf), "[CPUID] Step 1: EFLAGS before modification = 0x%08X\n", eflags_before);
    vga_write_color(buf, COLOR_LIGHT_GRAY, COLOR_BLACK);

    int id_bit_before = (eflags_before >> 21) & 1;
    str_format(buf, sizeof(buf), "[CPUID] Step 2: ID bit before = %d (bit 21)\n", id_bit_before);
    vga_write_color(buf, COLOR_LIGHT_GRAY, COLOR_BLACK);

    uint32_t modified = eflags_before ^ (1 << 21);
    asm volatile("pushl %0\n\t" "popfl" : : "r"(modified));

    str_format(buf, sizeof(buf), "[CPUID] Step 3: Toggled ID bit -> EFLAGS modified = 0x%08X\n", modified);
    vga_write_color(buf, COLOR_LIGHT_GRAY, COLOR_BLACK);

    asm volatile("pushfl\n\t" "popl %0" : "=r"(eflags_after));

    str_format(buf, sizeof(buf), "[CPUID] Step 4: EFLAGS after toggle = 0x%08X\n", eflags_after);
    vga_write_color(buf, COLOR_LIGHT_GRAY, COLOR_BLACK);

    int id_bit_after = (eflags_after >> 21) & 1;
    str_format(buf, sizeof(buf), "[CPUID] Step 5: ID bit after = %d\n", id_bit_after);
    vga_write_color(buf, COLOR_LIGHT_GRAY, COLOR_BLACK);

    int supported = ((eflags_after ^ eflags_before) & (1 << 21)) != 0;

    vga_write_color("[CPUID] Step 6: Comparing before/after EFLAGS...\n", COLOR_LIGHT_GRAY, COLOR_BLACK);

    if (supported)
    {
        print_status_ok();
        vga_write_color("CPU supports CPUID instruction.\n\n", COLOR_GREEN, COLOR_BLACK);
    }
    else
    {
        print_status_fail();
        vga_write_color("CPU does not support CPUID instruction!\n\n", COLOR_RED, COLOR_BLACK);
    }
}

static int detect_system_memory(uint32_t addr)
{
    vga_write_color("=> Detecting system memory... ", COLOR_WHITE, COLOR_BLACK);

    multiboot_info_t *mb_info = (multiboot_info_t *)addr;

    if (mb_info->flags & 0x001)
    {
        total_memory_mb = (mb_info->mem_lower + mb_info->mem_upper) / 1024;

        char mem_str[64];
        str_format(mem_str, sizeof(mem_str), "%d MB", total_memory_mb);

        print_status_ok();
        vga_write_color("Available RAM: ", COLOR_LIGHT_GRAY, COLOR_BLACK);
        vga_write_color(mem_str, COLOR_LIGHT_GRAY, COLOR_BLACK);
        vga_putc('\n');
        return 1;
    }
    else
    {
        print_status_fail();
        vga_write_color("Memory info not available.\n", COLOR_RED, COLOR_BLACK);
        return 0;
    }
}

static void start_init_system()
{
    vga_write_color("=> Starting init system...\n\n", COLOR_WHITE, COLOR_BLACK);
    system_init();
}

void kernel_main(uint32_t magic, uint32_t addr)
{
    print_kernel_info();
    check_cpu_mode();
    check_multiboot_signature(magic);
    check_smbios_presence();
    check_cpuid_support();
    detect_system_memory(addr);
    start_init_system();
}
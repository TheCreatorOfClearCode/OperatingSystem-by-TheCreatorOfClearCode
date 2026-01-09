#pragma once
#include <stdint.h>

typedef struct
{
    const char *vendor;
    const char *version;
    const char *date;
    uint32_t runtime_size;
    uint32_t rom_size;
    uint8_t bios_major, bios_minor;
    uint8_t fw_major, fw_minor;
} bios_info_t;

typedef struct
{
    const char *manufacturer;
    const char *product_name;
    const char *version;
    const char *serial;
    const char *uuid;
} system_info_t;

typedef struct
{
    const char *manufacturer;
    const char *product;
    const char *version;
    const char *serial;
} board_info_t;

void about_get_smbios_info(system_info_t *sys, board_info_t *board, bios_info_t *bios);
void about_get_cpu_info(uint32_t *logical, uint32_t *physical, uint32_t *base_freq, uint32_t *l1, uint32_t *l2, uint32_t *l3);
void about_get_cpu_brand(char *out);
void about_get_memory_info(uint32_t *total_mb);
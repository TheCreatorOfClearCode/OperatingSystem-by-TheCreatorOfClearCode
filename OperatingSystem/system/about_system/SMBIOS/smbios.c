#include "smbios.h"

static int smbios_checksum(uint8_t *addr, uint8_t len)
{
    uint8_t sum = 0;
    for (uint8_t i = 0; i < len; i++)
        sum += addr[i];
    return sum == 0;
}

static const char *smbios_get_string(uint8_t *struct_start, uint8_t index)
{
    if (index == 0)
        return "N/A";
    uint8_t len = struct_start[1];
    const char *strings = (const char *)(struct_start + len);
    uint8_t cur = 1;
    const char *s = strings;
    while (*s)
    {
        if (cur == index)
            return s;
        while (*s)
            s++;
        s++;
        cur++;
    }
    return "N/A";
}

void about_get_smbios_info(system_info_t *sys, board_info_t *board, bios_info_t *bios)
{
    uint8_t *base = (uint8_t *)0xF0000;

    for (uint32_t off = 0; off < 0x10000; off += 16)
    {
        uint8_t *p = base + off;
        if (p[0] == '_' && p[1] == 'S' && p[2] == 'M' && p[3] == '_')
        {
            if (!smbios_checksum(p, p[5]))
                continue;

            uint32_t table_addr = *(uint32_t *)(p + 0x18);
            uint16_t table_len = *(uint16_t *)(p + 0x16);
            uint8_t *tbl = (uint8_t *)table_addr;
            uint32_t pos = 0;

            while (pos < table_len)
            {
                uint8_t type = tbl[pos];
                uint8_t length = tbl[pos + 1];

                switch (type)
                {
                case 0:
                    bios->vendor = smbios_get_string(tbl + pos, tbl[pos + 4]);
                    bios->version = smbios_get_string(tbl + pos, tbl[pos + 5]);
                    bios->date = smbios_get_string(tbl + pos, tbl[pos + 8]);

                    if (length > 0x09)
                    {
                        uint8_t rom_size_raw = tbl[pos + 0x09];
                        bios->rom_size = (rom_size_raw + 1) * 64;
                    }
                    else
                    {
                        bios->rom_size = 0;
                    }

                    if (length > 0x18)
                    {
                        uint16_t runtime_size = *(uint16_t *)(tbl + pos + 0x17);
                        bios->runtime_size = runtime_size;
                    }
                    else
                    {
                        bios->runtime_size = 0;
                    }

                    if (length > 0x14)
                    {
                        bios->bios_major = tbl[pos + 0x14];
                        bios->bios_minor = tbl[pos + 0x15];
                    }
                    if (length > 0x16)
                    {
                        bios->fw_major = tbl[pos + 0x16];
                        bios->fw_minor = tbl[pos + 0x17];
                    }
                    break;

                case 1:
                    sys->manufacturer = smbios_get_string(tbl + pos, tbl[pos + 4]);
                    sys->product_name = smbios_get_string(tbl + pos, tbl[pos + 5]);
                    sys->version = smbios_get_string(tbl + pos, tbl[pos + 6]);
                    sys->serial = smbios_get_string(tbl + pos, tbl[pos + 7]);
                    sys->uuid = "Available";
                    break;
                case 2:
                    board->manufacturer = smbios_get_string(tbl + pos, tbl[pos + 4]);
                    board->product = smbios_get_string(tbl + pos, tbl[pos + 5]);
                    board->version = smbios_get_string(tbl + pos, tbl[pos + 6]);
                    board->serial = smbios_get_string(tbl + pos, tbl[pos + 7]);
                    break;
                }

                uint32_t next = pos + length;
                while (!(tbl[next] == 0 && tbl[next + 1] == 0))
                    next++;
                next += 2;
                pos = next;
            }
            return;
        }
    }
}
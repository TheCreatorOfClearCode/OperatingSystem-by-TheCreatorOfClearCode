#include "cpu_info.h"

void about_get_cpu_brand(char *out)
{
    uint32_t regs[4];
    for (int i = 0; i < 3; i++)
    {
        __asm__ __volatile__("cpuid" : "=a"(regs[0]), "=b"(regs[1]), "=c"(regs[2]), "=d"(regs[3]) : "a"(0x80000002 + i));
        ((uint32_t *)out)[i * 4 + 0] = regs[0];
        ((uint32_t *)out)[i * 4 + 1] = regs[1];
        ((uint32_t *)out)[i * 4 + 2] = regs[2];
        ((uint32_t *)out)[i * 4 + 3] = regs[3];
    }
    out[48] = '\0';
}

void about_get_cpu_info(uint32_t *logical, uint32_t *physical, uint32_t *base_freq, uint32_t *l1, uint32_t *l2, uint32_t *l3)
{
    uint32_t regs[4];

    __asm__ __volatile__("cpuid" : "=a"(regs[0]), "=b"(regs[1]), "=c"(regs[2]), "=d"(regs[3]) : "a"(1));
    *logical = (regs[1] >> 16) & 0xFF;

    if (*logical == 0)
    {
        *logical = 1;
    }

    __asm__ __volatile__("cpuid" : "=a"(regs[0]), "=b"(regs[1]), "=c"(regs[2]), "=d"(regs[3]) : "a"(4), "c"(0));
    *physical = ((regs[0] >> 26) & 0x3F) + 1;

    if (*logical < *physical)
    {
        *logical = *physical;
    }

    __asm__ __volatile__("cpuid" : "=a"(regs[0]), "=b"(regs[1]), "=c"(regs[2]), "=d"(regs[3]) : "a"(0x16));
    *base_freq = regs[0] & 0xFFFF;

    *l1 = *l2 = *l3 = 0;
    for (uint32_t i = 0;; i++)
    {
        __asm__ __volatile__("cpuid" : "=a"(regs[0]), "=b"(regs[1]), "=c"(regs[2]), "=d"(regs[3]) : "a"(4), "c"(i));
        uint32_t type = regs[0] & 0x1F;
        if (type == 0)
            break;
        uint32_t level = (regs[0] >> 5) & 0x7;
        uint32_t size = ((regs[1] >> 22) + 1) * ((regs[1] & 0x3FF) + 1) * ((regs[2] & 0xFFF) + 1) * 8 / 1024;
        if (level == 1)
            *l1 += size;
        else if (level == 2)
            *l2 += size;
        else if (level == 3)
            *l3 += size;
    }
}
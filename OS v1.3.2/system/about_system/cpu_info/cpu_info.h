#pragma once
#include <stdint.h>

void about_get_cpu_brand(char *out);
void about_get_cpu_info(uint32_t *logical, uint32_t *physical, uint32_t *base_freq, uint32_t *l1, uint32_t *l2, uint32_t *l3);
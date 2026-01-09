#pragma once
#include <stdint.h>

typedef struct
{
    const char *name;
    uint16_t port;
} io_port_t;

extern const io_port_t io_ports[];
extern const int io_port_count;
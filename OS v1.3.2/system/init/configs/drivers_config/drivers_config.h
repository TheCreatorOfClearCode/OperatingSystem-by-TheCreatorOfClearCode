#pragma once

typedef void (*driver_func_t)();

typedef enum
{
    DRIVER_PRIORITY_HIGH = 0,
    DRIVER_PRIORITY_NORMAL = 1,
    DRIVER_PRIORITY_LOW = 2
} driver_priority_t;

typedef struct
{
    const char *name;
    const char *description;
    driver_func_t init;
    driver_func_t stop;
    driver_priority_t priority;
} DriverConfig;

extern DriverConfig drivers[];
extern int drivers_count;
#ifndef NOTIFY_H
#define NOTIFY_H

#include "../../../system/kernel/drivers/VGA/vga.h"

typedef enum {
    NOTIFY_STANDARD_1,
    NOTIFY_STANDARD_2,
    NOTIFY_SUCCESS,
    NOTIFY_INFO_1,
    NOTIFY_INFO_2,
    NOTIFY_WARNING,
    NOTIFY_ERROR,
    NOTIFY_ERROR_DETAIL,
    NOTIFY_SYSTEM
} notify_type_t;

void notify(notify_type_t type, const char *message);

void notify_fmt(notify_type_t type, const char *format, ...);

#endif
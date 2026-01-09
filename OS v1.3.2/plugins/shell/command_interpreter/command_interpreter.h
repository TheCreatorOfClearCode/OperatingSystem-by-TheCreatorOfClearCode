#pragma once
#include <stddef.h>

typedef struct
{
    const char *name;
    void (*func)(const char *args);
    const char *description;
    const char *usage;
    int accepts_args;
} Command;

void register_command(const char *name, void (*func)(const char *args), const char *description, const char *usage, int accepts_args);
void execute_command(const char *buf);
void execute_command_by_index(size_t index, const char *args);

extern Command commands[];
extern size_t command_count;
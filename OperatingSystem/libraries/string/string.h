#ifndef STRING_H
#define STRING_H

#include <stddef.h>

size_t strlen(const char *str);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
char *strcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);
int strcasecmp(const char *s1, const char *s2);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *dest, int val, size_t len);
int str_format(char *buffer, size_t size, const char *fmt, ...);
int memcmp(const void *s1, const void *s2, size_t n);
char *strchr(const char *s, int c);
char *strstr(const char *haystack, const char *needle);
char *strncpy(char *dest, const char *src, size_t n);

#endif
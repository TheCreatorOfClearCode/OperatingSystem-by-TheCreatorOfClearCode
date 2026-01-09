#include "string.h"
#include <stdarg.h>

size_t strlen(const char *str)
{
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

int strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

char *strcpy(char *dest, const char *src)
{
    char *d = dest;
    while ((*d++ = *src++))
        ;
    return dest;
}

char *strcat(char *dest, const char *src)
{
    char *d = dest;
    while (*d)
        d++;
    while ((*d++ = *src++))
        ;
    return dest;
}

int strcasecmp(const char *s1, const char *s2)
{
    while (*s1 && *s2)
    {
        char c1 = *s1++;
        char c2 = *s2++;

        if (c1 >= 'A' && c1 <= 'Z')
            c1 += 32;
        if (c2 >= 'A' && c2 <= 'Z')
            c2 += 32;

        if (c1 != c2)
            return (unsigned char)c1 - (unsigned char)c2;
    }

    return (unsigned char)*s1 - (unsigned char)*s2;
}

void *memmove(void *dest, const void *src, size_t n)
{
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;

    if (d == s || n == 0)
        return dest;

    if (d < s)
    {
        for (size_t i = 0; i < n; i++)
            d[i] = s[i];
    }
    else
    {
        for (size_t i = n; i > 0; i--)
            d[i - 1] = s[i - 1];
    }

    return dest;
}

void *memset(void *dest, int val, size_t len)
{
    unsigned char *ptr = (unsigned char *)dest;
    while (len-- > 0)
        *ptr++ = (unsigned char)val;
    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *p1 = s1;
    const unsigned char *p2 = s2;

    for (size_t i = 0; i < n; i++)
    {
        if (p1[i] != p2[i])
            return p1[i] - p2[i];
    }
    return 0;
}

int str_format(char *buffer, size_t size, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    size_t pos = 0;
    for (const char *p = fmt; *p && pos < size - 1; p++)
    {
        if (*p == '%')
        {
            p++;
            int width = 0;
            int pad_zero = 0;

            if (*p == '0')
            {
                pad_zero = 1;
                p++;
                while (*p >= '0' && *p <= '9')
                {
                    width = width * 10 + (*p - '0');
                    p++;
                }
            }

            if (*p == 'd')
            {
                int val = va_arg(args, int);
                char num[32];
                int i = 0;

                if (val == 0)
                    num[i++] = '0';
                else
                {
                    int neg = 0;
                    if (val < 0)
                    {
                        neg = 1;
                        val = -val;
                    }
                    while (val && i < 31)
                    {
                        num[i++] = '0' + (val % 10);
                        val /= 10;
                    }
                    if (neg)
                        num[i++] = '-';
                }

                while (i < width)
                    num[i++] = pad_zero ? '0' : ' ';

                while (i-- > 0 && pos < size - 1)
                    buffer[pos++] = num[i];
            }
            else if (*p == 'x' || *p == 'X')
            {
                unsigned int val = va_arg(args, unsigned int);
                char num[32];
                int i = 0;
                const char *hex = (*p == 'x') ? "0123456789abcdef" : "0123456789ABCDEF";

                if (val == 0)
                    num[i++] = '0';
                else
                {
                    while (val && i < 31)
                    {
                        num[i++] = hex[val & 0xF];
                        val >>= 4;
                    }
                }

                while (i < width)
                    num[i++] = pad_zero ? '0' : ' ';

                while (i-- > 0 && pos < size - 1)
                    buffer[pos++] = num[i];
            }
            else if (*p == 's')
            {
                const char *s = va_arg(args, const char *);
                while (*s && pos < size - 1)
                    buffer[pos++] = *s++;
            }
            else if (*p == 'c')
            {
                char c = (char)va_arg(args, int);
                buffer[pos++] = c;
            }
            else if (*p == '%')
            {
                buffer[pos++] = '%';
            }
        }
        else
        {
            buffer[pos++] = *p;
        }
    }

    buffer[pos] = '\0';
    va_end(args);
    return (int)pos;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
    if (n == 0)
        return 0;

    while (n-- > 0 && *s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }

    if (n == (size_t)-1)
        return 0;

    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

char *strchr(const char *s, int c)
{
    while (*s != '\0')
    {
        if (*s == (char)c)
            return (char *)s;
        s++;
    }

    if (c == '\0')
        return (char *)s;

    return NULL;
}

char *strstr(const char *haystack, const char *needle)
{
    if (!*needle)
        return (char *)haystack;

    while (*haystack)
    {
        const char *h = haystack;
        const char *n = needle;

        while (*h && *n && (*h == *n))
        {
            h++;
            n++;
        }

        if (!*n)
            return (char *)haystack;
        haystack++;
    }

    return NULL;
}

char *strncpy(char *dest, const char *src, size_t n)
{
    char *d = dest;
    size_t i;

    for (i = 0; i < n && src[i] != '\0'; i++)
    {
        d[i] = src[i];
    }

    for (; i < n; i++)
    {
        d[i] = '\0';
    }

    return dest;
}
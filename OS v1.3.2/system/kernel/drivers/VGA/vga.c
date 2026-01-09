#include "vga.h"
#include "../IO/io.h"
#include "../../../../plugins/shell/notify/notify.h"
#include "../../../../libraries/string/string.h"

static uint16_t *const VGA_MEMORY = (uint16_t *)0xB8000;
static uint16_t buffer[VGA_HEIGHT][VGA_WIDTH];
static uint16_t old_buffer[VGA_HEIGHT][VGA_WIDTH];
static size_t row = 0, col = 0;
static uint8_t color;

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
    return (uint16_t)uc | (uint16_t)color << 8;
}

static inline uint8_t make_color(enum vga_color fg, enum vga_color bg)
{
    return fg | bg << 4;
}

void vga_setcolor(uint8_t fg, uint8_t bg)
{
    color = make_color(fg, bg);
}

uint8_t vga_getcolor()
{
    return color;
}

void move_cursor()
{ 
    uint16_t pos = row * VGA_WIDTH + col;
    outb(0x3D4, 14);
    outb(0x3D5, pos >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, pos & 0xFF);
}

void vga_enable_cursor(uint8_t start, uint8_t end)
{
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | start);
    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | end);
}

void vga_disable_cursor()
{
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void vga_update()
{
    for (size_t y = 0; y < VGA_HEIGHT; y++)
        for (size_t x = 0; x < VGA_WIDTH; x++)
            if (buffer[y][x] != old_buffer[y][x])
            {
                VGA_MEMORY[y * VGA_WIDTH + x] = buffer[y][x];
                old_buffer[y][x] = buffer[y][x];
            }
}

void scroll()
{
    if (row >= VGA_HEIGHT)
    {
        memmove(buffer, buffer[1], (VGA_HEIGHT - 1) * VGA_WIDTH * sizeof(uint16_t));
        for (size_t x = 0; x < VGA_WIDTH; x++)
            buffer[VGA_HEIGHT - 1][x] = vga_entry(' ', color);
        row = VGA_HEIGHT - 1;
        vga_update();
    }
}

typedef struct
{
    uint32_t unicode;
    unsigned char cp437;
} Utf8ToCp437Map;

static const Utf8ToCp437Map utf8_cp437_table[] = {
    {0x2588, 0xDB},
    {0x2593, 0xDC},
    {0x2592, 0xB1},
    {0x2591, 0xB0},
    {0x25A0, 0xFE},
    {0x25A1, 0xDB},
    {0x25C6, 0x04},
    {0x25CB, 0x09},
    {0x25CF, 0x0A},
    {0x2500, 0xC4},
    {0x2502, 0xB3},
    {0x250C, 0xDA},
    {0x2510, 0xBF},
    {0x2514, 0xC0},
    {0x2518, 0xD9},
    {0x251C, 0xC3},
    {0x2524, 0xB4},
    {0x252C, 0xC2},
    {0x2534, 0xC1},
    {0x253C, 0xC5},
    {0x2550, 0xCD},
    {0x2551, 0xBA},
    {0x2554, 0xC9},
    {0x2557, 0xBB},
    {0x255A, 0xC8},
    {0x255D, 0xBC},
    {0x2560, 0xCC},
    {0x2563, 0xB9},
    {0x2566, 0xCB},
    {0x2569, 0xCA},
    {0x256C, 0xCE},
    {0x2190, 0x1B},
    {0x2191, 0x18},
    {0x2192, 0x1A},
    {0x2193, 0x19},
    {0x2194, 0x1D},
    {0x2195, 0x12},
    {0x21A8, 0x17},
    {0x2665, 0x03},
    {0x2666, 0x04},
    {0x2663, 0x05},
    {0x2660, 0x06},
    {0x266A, 0x0D},
    {0x266B, 0x0E},
    {0x2319, 0xF9},
    {0x2302, 0x7F},
    {0x2219, 0xF9},
    {0x221A, 0xFB},
    {0x2248, 0xF7},
    {0x2264, 0xF3},
    {0x2265, 0xF2},
    {0x00B0, 0xF8},
    {0x00B1, 0xF1},
    {0x00B2, 0xFD},
    {0x00B7, 0xFA},
    {0x00F7, 0xF6},
    {0x256D, 0xD5},
    {0x256E, 0xB6},
    {0x256F, 0xB7},
    {0x2570, 0xD6},
    {0x2571, 0xD7},
    {0x2572, 0xD8},
    {0x2573, 0xD9}};

static unsigned char utf8_to_cp437(uint32_t codepoint)
{
    for (size_t i = 0; i < sizeof(utf8_cp437_table) / sizeof(utf8_cp437_table[0]); i++)
        if (utf8_cp437_table[i].unicode == codepoint)
            return utf8_cp437_table[i].cp437;
    return (codepoint < 128) ? (unsigned char)codepoint : '?';
}

static uint32_t utf8_decode_char(const char *s, size_t *bytes)
{
    unsigned char c = (unsigned char)s[0];
    if (c < 0x80)
    {
        *bytes = 1;
        return c;
    }
    else if ((c & 0xE0) == 0xC0)
    {
        *bytes = 2;
        return ((c & 0x1F) << 6) | (s[1] & 0x3F);
    }
    else if ((c & 0xF0) == 0xE0)
    {
        *bytes = 3;
        return ((c & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
    }
    else
    {
        *bytes = 1;
        return '?';
    }
}

static size_t utf8_strlen_visible(const char *s)
{
    size_t count = 0;
    while (*s)
    {
        size_t bytes;
        utf8_decode_char(s, &bytes);
        s += bytes;
        count++;
    }
    return count;
}

void vga_putc(char c)
{
    if (c == '\n')
    {
        col = 0;
        row++;
    }
    else if (c == '\r')
    {
        col = 0;
    }
    else
    {
        if (row < VGA_HEIGHT && col < VGA_WIDTH)
            buffer[row][col] = vga_entry(c, color);
        if (++col >= VGA_WIDTH)
        {
            col = 0;
            row++;
        }
    }
    scroll();
    vga_update();
    move_cursor();
}

void vga_putc_at(size_t r, size_t c, char ch) 
{
    if (r < VGA_HEIGHT && c < VGA_WIDTH) 
    {
        buffer[r][c] = vga_entry(ch, color);
        VGA_MEMORY[r * VGA_WIDTH + c] = buffer[r][c];
    }
}

void vga_write(const char *s)
{
    while (*s)
    {
        size_t bytes = 1;
        uint32_t code = utf8_decode_char(s, &bytes);
        vga_putc(utf8_to_cp437(code));
        s += bytes;
    }
}

void vga_write_color(const char *s, uint8_t fg, uint8_t bg)
{
    uint8_t old_color = vga_getcolor();
    vga_setcolor(fg, bg);
    vga_write(s);
    vga_setcolor(old_color & 0x0F, (old_color >> 4) & 0x0F);
}

void vga_write_color_centered(const char *s, uint8_t fg, uint8_t bg)
{
    size_t len = utf8_strlen_visible(s);
    size_t start_col = (VGA_WIDTH > len) ? (VGA_WIDTH - len) / 2 : 0;
    size_t cur_row = vga_get_row();
    vga_set_row_col(cur_row, start_col);
    vga_write_color(s, fg, bg);
    row++;
    col = 0;
}

void vga_write_color_centered_screen(const char **lines, size_t num_lines, uint8_t fg, uint8_t bg)
{
    if (num_lines == 0)
        return;
    size_t start_row = (VGA_HEIGHT > num_lines) ? (VGA_HEIGHT - num_lines) / 2 : 0;

    for (size_t i = 0; i < num_lines; i++)
    {
        const char *line = lines[i];
        size_t len = utf8_strlen_visible(line);
        size_t start_col = (VGA_WIDTH > len) ? (VGA_WIDTH - len) / 2 : 0;
        vga_set_row_col(start_row + i, start_col);
        vga_write_color(line, fg, bg);
    }

    vga_set_row_col(start_row + num_lines, 0);
}

void vga_write_centered(const char *s)
{
    size_t len = utf8_strlen_visible(s);
    size_t start_col = (VGA_WIDTH > len) ? (VGA_WIDTH - len) / 2 : 0;
    size_t cur_row = vga_get_row();
    vga_set_row_col(cur_row, start_col);
    vga_write(s);
    row++;
    col = 0;
}

void vga_write_centered_screen(const char **lines, size_t num_lines)
{
    if (num_lines == 0)
        return;
    size_t start_row = (VGA_HEIGHT > num_lines) ? (VGA_HEIGHT - num_lines) / 2 : 0;

    for (size_t i = 0; i < num_lines; i++)
    {
        const char *line = lines[i];
        size_t len = utf8_strlen_visible(line);
        size_t start_col = (VGA_WIDTH > len) ? (VGA_WIDTH - len) / 2 : 0;
        vga_set_row_col(start_row + i, start_col);
        vga_write(line);
    }

    vga_set_row_col(start_row + num_lines, 0);
}

void vga_clear()
{
    for (size_t y = 0; y < VGA_HEIGHT; y++)
        for (size_t x = 0; x < VGA_WIDTH; x++)
            buffer[y][x] = vga_entry(' ', color);
    row = col = 0;
    vga_update();
    move_cursor();
}

void vga_clear_area(size_t row_start, size_t row_end)
{
    if (row_end >= VGA_HEIGHT)
        row_end = VGA_HEIGHT - 1;

    uint8_t blank = vga_entry(' ', make_color(COLOR_BLACK, COLOR_BLACK));
    for (size_t y = row_start; y <= row_end; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            buffer[y][x] = blank;
        }
    }
    vga_update();
}

void vga_clear_char_at(size_t r, size_t c)
{
    if (r < VGA_HEIGHT && c < VGA_WIDTH)
    {
        buffer[r][c] = vga_entry(' ', color);
        vga_update();
    }
}

size_t vga_get_row() { return row; }
size_t vga_get_col() { return col; }

void vga_set_row_col(size_t r, size_t c)
{
    row = r;
    col = c;
    move_cursor();
}

static void vga_log(const char *msg)
{
    vga_write_color("[", COLOR_LIGHT_GRAY, COLOR_BLACK);
    vga_write_color("VGA", COLOR_CYAN, COLOR_BLACK);
    vga_write_color("] ", COLOR_LIGHT_GRAY, COLOR_BLACK);
    vga_write_color(msg, COLOR_WHITE, COLOR_BLACK);
}

void vga_init()
{
    notify(NOTIFY_SUCCESS, "=== Initializing VGA subsystem ===\n");

    vga_log("Setting color palette...");
    vga_setcolor(COLOR_LIGHT_GRAY, COLOR_BLACK);

    vga_putc('\n');

    vga_log("Enabling cursor...");
    vga_enable_cursor(14, 15);

    vga_putc('\n');

    notify(NOTIFY_SUCCESS, "=== VGA susbsystem initialization complete ===\n");
}

void vga_stop()
{
    vga_clear();
    vga_disable_cursor();
    memset(buffer, 0, sizeof(buffer));
    memset(old_buffer, 0, sizeof(old_buffer));
    outb(0x3D4, 0x00);
    outb(0x3D5, 0x00);
}
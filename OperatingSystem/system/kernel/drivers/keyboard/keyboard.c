#include "keyboard.h"
#include "../IO/io.h"
#include <stdint.h>

static uint8_t capslock = 0;
static uint8_t shift_pressed = 0;
static uint8_t ctrl_pressed = 0;
static uint8_t alt_pressed = 0;
static uint8_t keyboard_enabled = 0;

static const char scancode_table[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static const char shift_table[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int keyboard_getkey()
{
    if (!keyboard_enabled)
        return KEY_NONE;

    int key = KEY_NONE;

    while (key == KEY_NONE && keyboard_enabled)
    {
        if (inb(0x64) & 1)
        {
            uint8_t sc = inb(0x60);

            if (sc == 0x2A || sc == 0x36)
                shift_pressed = 1;
            else if (sc == 0xAA || sc == 0xB6)
                shift_pressed = 0;

            else if (sc == 0x1D)
                ctrl_pressed = 1;
            else if (sc == 0x9D)
                ctrl_pressed = 0;

            else if (sc == 0x38)
                alt_pressed = 1;
            else if (sc == 0xB8)
                alt_pressed = 0;

            else if (sc == 0x3A)
                capslock ^= 1;

            else if (sc == 0x01)
                key = KEY_ESC;

            else if (sc == 0x48)
                key = KEY_UP;
            else if (sc == 0x50)
                key = KEY_DOWN;
            else if (sc == 0x4B)
                key = KEY_LEFT;
            else if (sc == 0x4D)
                key = KEY_RIGHT;

            else if (sc == 0x0E)
                key = '\b';

            else if (sc < 128 && !(sc & 0x80))
            {
                char base = scancode_table[sc];
                char shifted = shift_table[sc];

                if (base >= 'a' && base <= 'z')
                {
                    if ((shift_pressed && !capslock) || (!shift_pressed && capslock) || (shift_pressed && capslock))
                        key = shifted;
                    else
                        key = base;
                }
                else
                {
                    key = shift_pressed ? shifted : base;
                }

                if (ctrl_pressed && key >= 'a' && key <= 'z')
                    key = 260 + (key - 'a');
            }
        }
    }

    return key;
}

void keyboard_init()
{
    capslock = 0;
    shift_pressed = 0;
    ctrl_pressed = 0;
    alt_pressed = 0;
    keyboard_enabled = 1;
}

void keyboard_stop()
{
    keyboard_enabled = 0;
    capslock = 0;
    shift_pressed = 0;
    ctrl_pressed = 0;
    alt_pressed = 0;

    outb(0x64, 0xAD);

    while (inb(0x64) & 1)
        inb(0x60);
}
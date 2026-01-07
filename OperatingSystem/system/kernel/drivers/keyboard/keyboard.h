#pragma once
#include <stdint.h>

#define KEY_NONE 0
#define KEY_UP 256
#define KEY_DOWN 257
#define KEY_LEFT 258
#define KEY_RIGHT 259
#define KEY_CTRL_A 260
#define KEY_ESC 261

void keyboard_init();
int keyboard_getkey();
void keyboard_stop();
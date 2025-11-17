// kernel/screen.c
#include "screen.h"
#include <stdint.h>

static volatile uint16_t *vga = (uint16_t*)0xB8000;
static int cursor = 0;
static uint8_t attribute = 0x07;

void screen_set_color(uint8_t fg, uint8_t bg) {
    attribute = (bg << 4) | (fg & 0x0F);
}

void screen_clear(void) {
    uint16_t blank = (uint16_t)attribute << 8 | (uint8_t)' ';
    for (int i = 0; i < 80*25; ++i) vga[i] = blank;
    cursor = 0;
}

void screen_putc(char c) {
    if (c == '\n') {
        cursor = (cursor/80 + 1) * 80;
        return;
    }
    vga[cursor++] = (uint16_t)attribute << 8 | (uint8_t)c;
    if (cursor >= 80*25) cursor = 0;
}

void screen_write(const char *s) {
    while (s && *s) screen_putc(*s++);
}


#ifndef SCREEN_H
#define SCREEN_H
#include <stdint.h>

void screen_set_color(uint8_t fg, uint8_t bg);
void screen_clear(void);
void screen_putc(char c);
void screen_write(const char *s);

#endif


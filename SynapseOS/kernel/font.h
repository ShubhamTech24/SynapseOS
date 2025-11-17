#ifndef FONT_H
#define FONT_H
#include <stdint.h>
#define FONT_WIDTH 8
#define FONT_HEIGHT 16

void font_init(void);
unsigned char* font_glyph(char c);
void draw_char(int x, int y, char c, uint32_t fg, uint32_t bg);
void draw_text(int x, int y, const char *s, uint32_t fg, uint32_t bg);
const uint8_t* font_get_char(char c);
#endif


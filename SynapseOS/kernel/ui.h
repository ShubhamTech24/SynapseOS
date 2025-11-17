#ifndef UI_H
#define UI_H

#include <stdint.h>   // ✅ fixes uint32_t not found
#include "framebuffer.h"
#include "colors.h"
#include "font.h"
void boot_splash(void);

void draw_char_scaled(int x, int y, char c, uint32_t fg, uint32_t bg, int scale);

/* Draw a filled rectangle */
void draw_rect(int x, int y, int w, int h, uint32_t color);

/* Draw bordered window-like rectangle */
void draw_border(int x, int y, int w, int h, uint32_t color);

/* Draw text string on screen with foreground/background color */
void draw_text(int x, int y, const char *s, uint32_t fg, uint32_t bg);

/* Draw a basic mouse cursor */
void draw_mouse_cursor(int x, int y, uint32_t color);

/* Main GUI function (draws desktop, window, etc.) */
void draw_gui(void);

/* Optional system info display (like neofetch-style) */
void draw_neofetch_ui(void);

#endif /* UI_H */

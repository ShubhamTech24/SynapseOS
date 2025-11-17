#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bpp;
    uint8_t *address;
} framebuffer_t;

extern framebuffer_t fb;

void fb_init(uint32_t w, uint32_t h, uint32_t pitch, uint32_t bpp, void *ptr);
void fb_init_from_mb2(uint32_t magic, uint32_t mb2_addr);

void *get_backbuffer(void);
void copy_to_screen(void);

void putpixel32(int x, int y, uint32_t color);
uint32_t getpixel32(int x, int y);

void fb_clear(uint32_t color);
void fb_fill_rect(int x, int y, int w, int h, uint32_t color);
void fb_draw_rect_outline(int x, int y, int w, int h, uint32_t color, int thickness);

uint32_t fb_get_width(void);
uint32_t fb_get_height(void);
uint32_t fb_get_pitch(void);
uint32_t fb_get_bpp(void);

uint32_t lerp_u32(uint32_t a, uint32_t b, float t);

#endif

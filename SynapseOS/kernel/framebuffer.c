// kernel/framebuffer.c
#include "framebuffer.h"
#include <stdint.h>
#include <stddef.h>
#include "string.h"     // for memcpy()
#include "colors.h"

framebuffer_t fb;
static uint8_t *draw_buffer = NULL;

/* ------------------------------------------------------------
 * Initialize framebuffer parameters
 * ------------------------------------------------------------ */
void fb_init(uint32_t w, uint32_t h, uint32_t pitch, uint32_t bpp, void *ptr) {
    fb.width = w;
    fb.height = h;
    fb.pitch = pitch;
    fb.bpp = bpp;
    fb.address = (uint8_t*)(uintptr_t)ptr;

    // Use backbuffer for smooth drawing if possible (double buffering)
    draw_buffer = (uint8_t*)(uintptr_t)fb.address;
}

/* Stub: Multiboot2 framebuffer init (you can expand if parsing tags) */
void fb_init_from_mb2(uint32_t magic, uint32_t mb2_addr) {
    (void)magic;
    (void)mb2_addr;
}

/* Return pointer to current drawing buffer */
void *get_backbuffer(void) {
    return (void*)draw_buffer;
}

/* ------------------------------------------------------------
 * Copy draw buffer to framebuffer memory (page flip)
 * ------------------------------------------------------------ */
void copy_to_screen(void) {
    if (!fb.address || !draw_buffer) return;
    uint8_t *dst_base = (uint8_t*)(uintptr_t)fb.address;
    for (uint32_t y = 0; y < fb.height; ++y) {
        uint8_t *src = draw_buffer + y * fb.pitch;
        uint8_t *dst = dst_base + y * fb.pitch;
        memcpy(dst, src, fb.pitch);
    }
}

/* ------------------------------------------------------------
 * Write a pixel (32-bit RGBA)
 * ------------------------------------------------------------ */
void putpixel32(int x, int y, uint32_t color) {
    if (!fb.address || fb.bpp != 32) return;
    if (x < 0 || y < 0 || (uint32_t)x >= fb.width || (uint32_t)y >= fb.height) return;
    uint8_t *base = (uint8_t*)(uintptr_t)fb.address;
    uint32_t *pixel = (uint32_t*)(uintptr_t)(base + (uint64_t)y * fb.pitch + (uint64_t)x * 4);
    *pixel = color;
}

/* ------------------------------------------------------------
 * Read pixel color from framebuffer (for fade effects, etc.)
 * ------------------------------------------------------------ */
uint32_t getpixel32(int x, int y) {
    if (!fb.address || fb.bpp != 32) return 0;
    if (x < 0 || y < 0 || (uint32_t)x >= fb.width || (uint32_t)y >= fb.height) return 0;
    uint8_t *base = (uint8_t*)(uintptr_t)fb.address;
    uint32_t *pixel = (uint32_t*)(uintptr_t)(base + (uint64_t)y * fb.pitch + (uint64_t)x * 4);
    return *pixel;
}

/* ------------------------------------------------------------
 * Fill entire framebuffer with a color
 * ------------------------------------------------------------ */
void fb_clear(uint32_t color) {
    if (!fb.address || fb.bpp != 32) return;
    for (uint32_t y = 0; y < fb.height; ++y) {
        uint32_t *row = (uint32_t*)(uintptr_t)((uint8_t*)(uintptr_t)fb.address + (uint64_t)y * fb.pitch);
        for (uint32_t x = 0; x < fb.width; ++x)
            row[x] = color;
    }
}

/* ------------------------------------------------------------
 * Draw a filled rectangle
 * ------------------------------------------------------------ */
void fb_fill_rect(int x, int y, int w, int h, uint32_t color) {
    if (!fb.address || fb.bpp != 32) return;
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > (int)fb.width) w = fb.width - x;
    if (y + h > (int)fb.height) h = fb.height - y;
    if (w <= 0 || h <= 0) return;

    for (int yy = 0; yy < h; ++yy) {
        uint32_t *row = (uint32_t*)(uintptr_t)((uint8_t*)(uintptr_t)fb.address +
                        (uint64_t)(y + yy) * fb.pitch + (uint64_t)x * 4);
        for (int xx = 0; xx < w; ++xx)
            row[xx] = color;
    }
}

/* ------------------------------------------------------------
 * Draw rectangle border (for windows, boxes, etc.)
 * ------------------------------------------------------------ */
void fb_draw_rect_outline(int x, int y, int w, int h, uint32_t color, int thickness) {
    fb_fill_rect(x, y, w, thickness, color);                     // top
    fb_fill_rect(x, y + h - thickness, w, thickness, color);     // bottom
    fb_fill_rect(x, y, thickness, h, color);                     // left
    fb_fill_rect(x + w - thickness, y, thickness, h, color);     // right
}

/* ------------------------------------------------------------
 * Linear interpolation between two colors (for gradients)
 * ------------------------------------------------------------ */


/* ------------------------------------------------------------
 * Framebuffer info getters
 * ------------------------------------------------------------ */
uint32_t fb_get_width(void)  { return fb.width;  }
uint32_t fb_get_height(void) { return fb.height; }
uint32_t fb_get_pitch(void)  { return fb.pitch;  }
uint32_t fb_get_bpp(void)    { return fb.bpp;    }

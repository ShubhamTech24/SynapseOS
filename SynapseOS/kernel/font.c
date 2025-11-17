// kernel/font.c - uses objcopy-embedded PSF
#include "font.h"
#include "framebuffer.h"
#include <stdint.h>

extern unsigned char _binary_kernel_font_psf_start[];
extern unsigned char _binary_kernel_font_psf_end[];
extern unsigned char _binary_kernel_font_psf_size[];
typedef struct {
    uint8_t magic[2];
    uint8_t mode;
    uint8_t charsize;
} psf1_header_t;

static psf1_header_t *font_hdr = 0;
static unsigned char *glyphs = 0;

void font_init(void) {
    unsigned char *data = _binary_kernel_font_psf_start;
    font_hdr = (psf1_header_t*)data;
    glyphs = data + sizeof(psf1_header_t);
}

unsigned char* font_glyph(char c) {
    if (!font_hdr) font_init();
    return glyphs + ((unsigned char)c * font_hdr->charsize);
}

void draw_char(int x, int y, char c, uint32_t fg, uint32_t bg) {
    if (!font_hdr) font_init();
    unsigned char *g = font_glyph(c);
    for (int row=0; row < (int)font_hdr->charsize; ++row) {
        unsigned char bits = g[row];
        for (int col=0; col<8; ++col) {
            uint32_t color = (bits & (0x80 >> col)) ? fg : bg;
            putpixel32(x + col, y + row, color);
        }
    }
}

void draw_text(int x, int y, const char *s, uint32_t fg, uint32_t bg) {
    while (s && *s) {
        draw_char(x, y, *s++, fg, bg);
        x += 8;
    }
}
/* Returns pointer to 8x16 glyph data for given ASCII char */
const uint8_t* font_get_char(char c) {
    if ((unsigned char)c < 32)
        c = 32; // replace control chars with space

    // Use the PSF font data loaded via objcopy
    uint8_t *font_data = _binary_kernel_font_psf_start;
    return font_data + (c - 32) * 16; // each glyph = 16 bytes (8x16 font)
}


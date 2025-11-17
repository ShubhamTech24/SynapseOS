#include "boot_splash.h"
#include "framebuffer.h"
#include "font.h"
#include "colors.h"
#include "ui.h"
#include "string.h"

/* Utility: simple delay loop (CPU-busy wait) */
static void delay(int ms) {
    for (volatile int i = 0; i < ms * 8000; i++);
}

/* Helper: get text width in pixels (approx, assuming 8px monospace font) */
static int text_width(const char *text) {
    return strlen(text) * 8;  // if using 8x16 PSF font
}

/* Boot splash with fade + loading animation */
void boot_splash(void) {
    if (!fb.address || fb.bpp != 32) return;

    const char *title = "SynapseOS";
    const char *subtitle = "v1.0   Booting...";

    int title_w = text_width(title);
    int sub_w   = text_width(subtitle);

    /* Perfect centering on any resolution */
    int title_x = (fb.width  - title_w) / 2;
    int title_y = (fb.height / 2) - 20;

    int sub_x = (fb.width  - sub_w) / 2;
    int sub_y = title_y + 40;

    /* Fade-in background gradient */
    for (int alpha = 0; alpha <= 255; alpha += 5) {
        for (uint32_t y = 0; y < fb.height; y++) {
            float t = (float)y / (float)fb.height;
            uint32_t col = lerp_u32(0xFF000000, COLOR_DARKBLUE, t);
            uint8_t r = (col >> 16) & 0xFF;
            uint8_t g = (col >> 8) & 0xFF;
            uint8_t b = col & 0xFF;
            uint32_t fade = (0xFF << 24) | ((r * alpha / 255) << 16) | ((g * alpha / 255) << 8) | (b * alpha / 255);
            for (uint32_t x = 0; x < fb.width; x++) putpixel32(x, y, fade);
        }
        copy_to_screen();
    }

    /* Draw glowing SynapseOS title pulse */
    for (int pulse = 0; pulse < 3; pulse++) {
        uint32_t color = (pulse % 2 == 0) ? COLOR_CYAN : COLOR_LIGHTBLUE;
        draw_text(title_x, title_y, title, color, COLOR_BLACK);
        draw_text(sub_x, sub_y, subtitle, COLOR_WHITE, COLOR_BLACK);
        copy_to_screen();
        delay(200);
    }

    /* Animated loading bar (centered too) */
    int bar_w = 300, bar_h = 12;
    int bar_x = (fb.width - bar_w) / 2;
    int bar_y = sub_y + 60;
    fb_fill_rect(bar_x - 2, bar_y - 2, bar_w + 4, bar_h + 4, COLOR_DARKGRAY);

    for (int i = 0; i <= bar_w; i += 6) {
        fb_fill_rect(bar_x, bar_y, i, bar_h, COLOR_CYAN);
        copy_to_screen();
        delay(10);
    }

    /* Hold splash for 1.5 seconds */
    delay(1500);

    /* Fade-out transition */
    for (int alpha = 255; alpha >= 0; alpha -= 8) {
        for (uint32_t y = 0; y < fb.height; y++) {
            for (uint32_t x = 0; x < fb.width; x++) {
                uint32_t c = getpixel32(x, y);
                uint8_t r = (c >> 16) & 0xFF;
                uint8_t g = (c >> 8) & 0xFF;
                uint8_t b = c & 0xFF;
                uint32_t faded = (0xFF << 24) | ((r * alpha / 255) << 16) |
                                 ((g * alpha / 255) << 8) | (b * alpha / 255);
                putpixel32(x, y, faded);
            }
        }
        copy_to_screen();
    }
}

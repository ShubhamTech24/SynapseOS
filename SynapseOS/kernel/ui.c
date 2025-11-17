// kernel/ui.c - Advanced SynapseOS GUI with Notepad integration
#include "ui.h"
#include "framebuffer.h"
#include "font.h"
#include "colors.h"
#include "system_info.h"
#include "notepad.h"    // added
#include <stdint.h>

#define CURSOR_W 8
#define CURSOR_H 8
#define POPUP_FADE_STEPS 20
#define POPUP_DELAY 90000000

extern framebuffer_t fb;

/* -------------------------------------------------------------------------
 * Cursor handling
 * ------------------------------------------------------------------------- */
static uint32_t cursor_bg[CURSOR_W * CURSOR_H];
static int last_x = -1, last_y = -1;

/* Save background under the cursor */
static void save_bg(int x, int y) {
    for (int yy = 0; yy < CURSOR_H; yy++) {
        for (int xx = 0; xx < CURSOR_W; xx++) {
            if (x + xx < (int)fb.width && y + yy < (int)fb.height) {
                uint32_t *row = (uint32_t *)(fb.address + (y + yy) * fb.pitch);
                cursor_bg[yy * CURSOR_W + xx] = row[x + xx];
            }
        }
    }
}

/* Restore background under the old cursor */
static void restore_bg(void) {
    if (last_x < 0 || last_y < 0) return;
    for (int yy = 0; yy < CURSOR_H; yy++) {
        for (int xx = 0; xx < CURSOR_W; xx++) {
            if (last_x + xx < (int)fb.width && last_y + yy < (int)fb.height) {
                uint32_t *row = (uint32_t *)(fb.address + (last_y + yy) * fb.pitch);
                row[last_x + xx] = cursor_bg[yy * CURSOR_W + xx];
            }
        }
    }
}

/* Draw a smooth triangular cursor */
void draw_mouse_cursor(int x, int y, uint32_t color) {
    if (!fb.address) return;

    restore_bg();
    save_bg(x, y);
    last_x = x; last_y = y;

    for (int yy = 0; yy < CURSOR_H; yy++) {
        for (int xx = 0; xx <= yy; xx++) {
            if (x + xx < (int)fb.width && y + yy < (int)fb.height) {
                uint32_t *row = (uint32_t *)(fb.address + (y + yy) * fb.pitch);
                row[x + xx] = color;
            }
        }
    }
}

/* Draw a highlight outline box */
void draw_highlight_box(int x, int y, int w, int h, uint32_t color) {
    fb_fill_rect(x - 2, y - 2, w + 4, h + 4, color);
}

/* -------------------------------------------------------------------------
 * GUI Elements
 * ------------------------------------------------------------------------- */

void draw_neofetch_ui(void) { draw_gui(); }

/* Main dashboard UI */
void draw_gui(void) {
    if (!fb.address || fb.bpp != 32) return;

    // Background gradient
    for (uint32_t y = 0; y < fb.height; y++) {
        float t = (float)y / (float)fb.height;
        uint32_t col = lerp_u32(COLOR_DARKGRAY, COLOR_BLACK, t);
        for (uint32_t x = 0; x < fb.width; x++)
            putpixel32(x, y, col);
    }

    // Top bar
    fb_fill_rect(0, 0, fb.width, 44, COLOR_DARKBLUE);
    draw_text(16, 12, "SynapseOS", COLOR_CYAN, COLOR_DARKBLUE);
    draw_text(fb.width - 300, 12, "v1.0", COLOR_LIGHTGRAY, COLOR_DARKBLUE);

    // Left dock
    int dock_w = 80;
    fb_fill_rect(0, 44, dock_w, fb.height - 44, COLOR_BLACK);
    for (int i = 0; i < 6; i++) {
        int x = 12, y = 60 + i * 90;
        uint32_t color = COLOR_GRAY;
        if (i == 0) color = COLOR_BLUE; // notepad icon highlighted
        fb_fill_rect(x, y, 56, 56, color);
    }

    // Central dashboard card
    int w = fb.width * 60 / 100;
    int h = fb.height * 50 / 100;
    int x = (fb.width - w) / 2;
    int y = 110;

    fb_fill_rect(x + 8, y + 8, w, h, 0xFF0A0A0A);  // shadow
    fb_fill_rect(x, y, w, h, COLOR_DARKGRAY);
    fb_fill_rect(x, y, w, 40, COLOR_BLUE);
    draw_text(x + 12, y + 10, "Dashboard", COLOR_WHITE, COLOR_BLUE);
    draw_text(x + 20, y + 60, "System Monitor Active", COLOR_LIGHTGRAY, COLOR_DARKGRAY);

    // Input field
    int box_x = fb.width / 2 - 300;
    int box_y = fb.height / 2 - 40;
    fb_fill_rect(box_x, box_y, 600, 80, COLOR_DARKGRAY);
    draw_text(box_x + 15, box_y + 30, "Type here...", COLOR_WHITE, COLOR_DARKGRAY);

    // Dynamic system info
    system_info_draw();

    // If notepad is open, draw it on top of the GUI
    if (notepad_is_open())
        notepad_draw();

    copy_to_screen();
}

/* -------------------------------------------------------------------------
 * Popup and click logic
 * ------------------------------------------------------------------------- */

/* Fade-in popup window */
static void popup_fade_in(int x, int y, int w, int h, uint32_t bg_color) {
    for (int a = 0; a <= 255; a += (255 / POPUP_FADE_STEPS)) {
        uint32_t faded = (0xFF << 24) |
                         (((bg_color >> 16) & 0xFF) * a / 255 << 16) |
                         (((bg_color >> 8) & 0xFF) * a / 255 << 8) |
                         ((bg_color & 0xFF) * a / 255);
        fb_fill_rect(x, y, w, h, faded);
        copy_to_screen();
    }
}

/* Popup window */
void show_popup(const char *title, const char *message) {
    int w = 320, h = 180;
    int x = (fb.width - w) / 2;
    int y = (fb.height - h) / 2;

    fb_fill_rect(x + 6, y + 6, w, h, COLOR_BLACK);
    popup_fade_in(x, y, w, h, COLOR_DARKGRAY);
    fb_fill_rect(x, y, w, 30, COLOR_BLUE);

    draw_text(x + 10, y + 8, title, COLOR_WHITE, COLOR_BLUE);
    draw_text(x + 12, y + 55, message, COLOR_LIGHTGRAY, COLOR_DARKGRAY);
    copy_to_screen();

    // delay to hold popup
    for (volatile int d = 0; d < POPUP_DELAY; d++);
    draw_gui(); // redraw UI after popup closes
}

/* Handle dock icon click */
void handle_mouse_click(int x, int y) {
    if (x >= 12 && x <= 68) {
        int icon_index = (y - 60) / 90;

        // icon 0 → Notepad
        if (icon_index == 0) {
            notepad_open();
            return;
        }
        else if (icon_index == 1)
            show_popup("About", "SynapseOS v1.0\nAI Enhanced Kernel UI");
        else if (icon_index == 2)
            show_popup("Network", "No active connection");
        else if (icon_index == 3)
            show_popup("Storage", "Disk 120GB\nUsage: 54%");
        else if (icon_index == 4)
            show_popup("Settings", "Coming Soon...");
    }
}

/* Draw scaled character (for large logo, titles, etc.) */
void draw_char_scaled(int x, int y, char c, uint32_t fg, uint32_t bg, int scale) {
    const uint8_t *glyph = font_get_char(c);
    for (int row = 0; row < 16; row++) {
        uint8_t bits = glyph[row];
        for (int col = 0; col < 8; col++) {
            uint32_t color = (bits & (1 << (7 - col))) ? fg : bg;
            for (int sy = 0; sy < scale; sy++)
                for (int sx = 0; sx < scale; sx++)
                    putpixel32(x + col * scale + sx, y + row * scale + sy, color);
        }
    }
}

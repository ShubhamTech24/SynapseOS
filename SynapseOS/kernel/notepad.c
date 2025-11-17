

// kernel/notepad.c — SynapseOS Notepad App
#include "notepad.h"
#include "framebuffer.h"
#include "font.h"
#include "colors.h"
#include "ui.h"
#include "screen.h"
#include <stdint.h>
#include <string.h>   // for strcat, strcpy, memset

/* -------------------------------------------------------------------------
 * Window geometry
 * ------------------------------------------------------------------------- */
#define NOTEPAD_W 640
#define NOTEPAD_H 420
#define NOTEPAD_X ((int)(fb.width / 2 - NOTEPAD_W / 2))
#define NOTEPAD_Y ((int)(fb.height / 2 - NOTEPAD_H / 2))
#define FONT_WIDTH 8
#define FONT_HEIGHT 16

/* -------------------------------------------------------------------------
 * Notepad state
 * ------------------------------------------------------------------------- */
static int is_open = 0;
static char lines[64][128];
static int num_lines = 1;
static int caret_row = 0, caret_col = 0;
static char buf[BUF_CAPACITY];

/* -------------------------------------------------------------------------
 * Internal helper functions
 * ------------------------------------------------------------------------- */
static void notepad_draw_contents(void) {
    int area_x = NOTEPAD_X + 8;
    int area_y = NOTEPAD_Y + 36;
    int area_w = NOTEPAD_W - 16;
    int area_h = NOTEPAD_H - 44;

    fb_fill_rect(area_x, area_y, area_w, area_h, COLOR_DARKGRAY);

    for (int i = 0; i < num_lines; ++i) {
        draw_text(area_x + 4, area_y + i * FONT_HEIGHT, lines[i], COLOR_WHITE, COLOR_DARKGRAY);
    }

    // Draw caret
    int caret_px = area_x + 4 + caret_col * FONT_WIDTH;
    int caret_py = area_y + caret_row * FONT_HEIGHT;
    fb_fill_rect(caret_px, caret_py, 2, FONT_HEIGHT, COLOR_CYAN);
    copy_to_screen();
}

/* -------------------------------------------------------------------------
 * Public API
 * ------------------------------------------------------------------------- */
void notepad_open(void) {
    memset(lines, 0, sizeof(lines));
    num_lines = 1;
    caret_row = caret_col = 0;
    is_open = 1;
    notepad_draw();
}

void notepad_close(void) {
    is_open = 0;
    draw_gui(); // restore desktop UI
}

int notepad_is_open(void) { return is_open; }

void notepad_draw(void) {
    fb_fill_rect(NOTEPAD_X, NOTEPAD_Y, NOTEPAD_W, NOTEPAD_H, COLOR_LIGHTGRAY);
    fb_fill_rect(NOTEPAD_X, NOTEPAD_Y, NOTEPAD_W, 28, COLOR_BLUE);
    draw_text(NOTEPAD_X + 8, NOTEPAD_Y + 6, "Notepad - SynapseOS", COLOR_WHITE, COLOR_BLUE);
    draw_text(NOTEPAD_X + NOTEPAD_W - 180, NOTEPAD_Y + 6, "[Ctrl+S] Save  [Esc] Close", COLOR_WHITE, COLOR_BLUE);
    notepad_draw_contents();
}

/* -------------------------------------------------------------------------
 * Input handling
 * ------------------------------------------------------------------------- */
void notepad_handle_key(char c) {
    if (!is_open) return;
    if (c == '\n') {
        if (num_lines < 64) {
            caret_row++;
            num_lines++;
            caret_col = 0;
        }
    } else {
        int len = strlen(lines[caret_row]);
        if (len < 127) {
            lines[caret_row][len] = c;
            lines[caret_row][len + 1] = '\0';
            caret_col++;
        }
    }
    notepad_draw_contents();
}

void notepad_handle_keycode(uint8_t code) {
    if (!is_open) return;

    switch (code) {
        case 8: // Backspace
            if (caret_col > 0) {
                caret_col--;
                lines[caret_row][caret_col] = '\0';
            } else if (caret_row > 0) {
                // merge with previous line
                strcat(lines[caret_row - 1], lines[caret_row]);
                for (int i = caret_row; i < num_lines - 1; ++i)
                    strcpy(lines[i], lines[i + 1]);
                num_lines--;
                caret_row--;
                caret_col = strlen(lines[caret_row]);
            }
            break;

        case 13: // Enter
            if (num_lines < 64) {
                for (int i = num_lines; i > caret_row + 1; --i)
                    strcpy(lines[i], lines[i - 1]);
                num_lines++;
                caret_row++;
                lines[caret_row][0] = '\0';
                caret_col = 0;
            }
            break;

        case 19: // Ctrl+S (Save)
            buf[0] = '\0';
            for (int i = 0; i < num_lines; ++i) {
                strcat(buf, lines[i]);
                strcat(buf, "\n");
            }
            fb_fill_rect(NOTEPAD_X, NOTEPAD_Y, NOTEPAD_W, 28, COLOR_GREEN);
            draw_text(NOTEPAD_X + 10, NOTEPAD_Y + 6, "Saved to memory!", COLOR_BLACK, COLOR_GREEN);
            copy_to_screen();
            break;

        case 27: // Esc (Close)
            notepad_close();
            return;
    }

    notepad_draw_contents();
}

/* -------------------------------------------------------------------------
 * Retrieve current text buffer
 * ------------------------------------------------------------------------- */
const char *notepad_get_text(void) {
    buf[0] = '\0';
    for (int i = 0; i < num_lines; ++i) {
        strcat(buf, lines[i]);
        strcat(buf, "\n");
    }
    return buf;
}

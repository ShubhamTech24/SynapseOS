// kernel/keyboard.c — SynapseOS unified keyboard handler with Notepad integration

#include "screen.h"
#include "framebuffer.h"
#include "font.h"
#include "ui.h"
#include "scancode.h"
#include "colors.h"
#include "ports.h"
#include "notepad.h"   // added for notepad support
#include <stdint.h>

#define MAX_INPUT 128

static int term_x = 320, term_y = 220;
static int cursor_x = 0;
static int cursor_y = 0;
static char input_buffer[MAX_INPUT];
static int input_len = 0;

/* -------------------------------------------------------------------------
 * Initialization
 * ------------------------------------------------------------------------- */
void keyboard_init(void) {
    screen_write("Keyboard initialized (polling mode)\n");
}

/* Simplified scancode to ASCII map — US QWERTY */
static const unsigned char scancode_ascii[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' ',
    /* rest = 0 */
};

/* -------------------------------------------------------------------------
 * Keyboard polling and handling
 * ------------------------------------------------------------------------- */
void keyboard_poll_and_handle(void) {
    uint8_t status = inb(0x64);

    // Check if keyboard data is available (bit 0) and not from mouse (bit 5)
    if (!(status & 1) || (status & 0x20))
        return;

    uint8_t sc = inb(0x60);

    // Ignore key releases (bit 7 set)
    if (sc & 0x80)
        return;

    unsigned char c = scancode_ascii[sc];

    /* -----------------------------------------------------------------
     * Special key detection (Enter, Backspace, Ctrl+S, Esc)
     * ----------------------------------------------------------------- */
    if (sc == 14) { // Backspace
        if (notepad_is_open()) {
            notepad_handle_keycode(8);
            return;
        } else if (input_len > 0) {
            input_buffer[--input_len] = '\0';
        }
    }
    else if (sc == 28) { // Enter key
        if (notepad_is_open()) {
            notepad_handle_keycode(13);
            return;
        }
        input_buffer[input_len++] = '\n';
        input_buffer[input_len] = '\0';
    }
    else if (sc == 1) { // Esc
        if (notepad_is_open()) {
            notepad_handle_keycode(27);
            return;
        }
        // Could later add menu close / UI reset here
    }
    else if (sc == 31) { // Ctrl+S (scancode of 'S' key, check Ctrl manually if needed)
        uint8_t ctrl = inb(0x60); // placeholder — depends on your scancode map
        if (notepad_is_open()) {
            notepad_handle_keycode(19); // Ctrl+S save
            return;
        }
    }
    else if (c >= 32 && c <= 126) {
        /* Normal printable characters */
        if (notepad_is_open()) {
            notepad_handle_key(c);
            return;
        } else {
            if (input_len < MAX_INPUT - 1) {
                input_buffer[input_len++] = c;
                input_buffer[input_len] = '\0';
            }
        }
    }

    /* -----------------------------------------------------------------
     * Redraw input box when notepad is closed
     * ----------------------------------------------------------------- */
    if (!notepad_is_open()) {
        int box_x = fb.width / 2 - 300;
        int box_y = fb.height / 2 - 40;
        fb_fill_rect(box_x, box_y, 600, 80, COLOR_DARKGRAY);
        draw_text(box_x + 15, box_y + 30, input_buffer, COLOR_WHITE, COLOR_DARKGRAY);
        copy_to_screen();
    }
}

// kernel/mouse.c — SynapseOS interactive mouse driver (fixed)
#include "mouse.h"
#include "ports.h"        // inb/outb functions
#include "framebuffer.h"  // fb, fb.width, fb.height
#include "ui.h"           // handle_mouse_click(), draw_mouse_cursor()
#include "screen.h"       // for debug screen_write()
#include "colors.h"
#include <stdint.h>

#define MOUSE_SENSITIVITY 2.2f
#define MOUSE_LEFT_BTN 0x1
void handle_mouse_click(int x, int y);  // forward declaration

static int mx = 50, my = 50;                      /* cursor position */
static uint32_t cursor_color = COLOR_WHITE;       /* default cursor color */
static uint8_t packet[3];                         /* PS/2 packet buffer */
static int packet_i = 0;                          /* byte counter */
static int mouse_enabled = 0;
static uint8_t last_buttons = 0;                  /* to detect click edges */

/* Wait until data is available from the PS/2 controller */
static int mouse_data_available(void) {
    return (inb(0x64) & 1);
}

/* PS/2 write helpers */
static void ps2_write_cmd(uint8_t cmd) {
    while (inb(0x64) & 2) { /* wait */ }
    outb(0x64, cmd);
}

static void ps2_write_data(uint8_t data) {
    while (inb(0x64) & 2) { /* wait */ }
    outb(0x60, data);
}

static uint8_t ps2_read_data(void) {
    return inb(0x60);
}

/* Initialize PS/2 mouse device */
void mouse_init(void) {
    ps2_write_cmd(0xA8);      // Enable auxiliary device (mouse)
    ps2_write_cmd(0xD4);
    ps2_write_data(0xF6);     // Set default settings
    for (volatile int i = 0; i < 10000; i++);

    ps2_write_cmd(0xD4);
    ps2_write_data(0xF4);     // Enable data reporting
    for (volatile int i = 0; i < 10000; i++);

    mouse_enabled = 1;
    screen_write("Mouse initialized (interactive mode)\n");
}

/* Poll mouse data: movement + clicks */
void mouse_poll_update(void) {
    if (!mouse_enabled)
        return;

    while (mouse_data_available()) {
        uint8_t b = ps2_read_data();

        // Sync packet (bit 3 should always be 1 for first byte)
        if (packet_i == 0 && !(b & 0x08))
            continue;

        packet[packet_i++] = b;

        if (packet_i == 3) {
            int8_t dx = (int8_t)packet[1];
            int8_t dy = (int8_t)packet[2];
            uint8_t buttons = packet[0];
            (void)buttons; // prevent unused variable warning

            /* Update mouse position (invert dy: PS/2 uses opposite Y direction) */
            mx += (int)((float)dx * MOUSE_SENSITIVITY);
            my -= (int)((float)dy * MOUSE_SENSITIVITY);

            /* Clamp cursor inside framebuffer bounds */
            if (mx < 0) mx = 0;
            if (my < 0) my = 0;
            if (mx >= (int)fb.width - 8) mx = (int)fb.width - 8;
            if (my >= (int)fb.height - 8) my = (int)fb.height - 8;

            /* Draw cursor */
            draw_mouse_cursor(mx, my, cursor_color);

            /* Detect left click press (edge detection) */
            if ((packet[0] & MOUSE_LEFT_BTN) && !(last_buttons & MOUSE_LEFT_BTN)) {
                // Pass current mouse position to UI click handler
                handle_mouse_click(mx, my);
            }

            last_buttons = packet[0];
            packet_i = 0; // reset packet index
        }
    }
}

/* Allow external code to change cursor color */
void mouse_set_color(uint32_t color) {
    cursor_color = color;
}

#include "system_info.h"
#include "framebuffer.h"
#include "ui.h"
#include "colors.h"
#include "string.h"

static int cpu_usage = 12;
static int ram_used = 3400; // in MB
static int ram_total = 8192;

/* Small helper to simulate dynamic CPU and RAM behavior */
void system_info_update(void) {
    static int dir = 1;
    cpu_usage += dir * 2;
    if (cpu_usage > 92 || cpu_usage < 10)
        dir = -dir;

    // Simulate slow RAM fluctuation
    ram_used += dir * 25;
    if (ram_used < 2500) ram_used = 2500;
    if (ram_used > 7000) ram_used = 7000;
}

/* Draw the system info cards on the right panel */
void system_info_draw(void) {
    int wx = fb.width - 240;
    int wy = 150;
    int card_w = 200;
    int card_h = 100;

    // Background box
    fb_fill_rect(wx, wy, card_w, card_h, COLOR_DARKBLUE);

    char buffer[64];
    mini_snprintf(buffer, sizeof(buffer), "CPU: %d%%", cpu_usage);
    draw_text(wx + 12, wy + 12, buffer, COLOR_WHITE, COLOR_DARKBLUE);

    mini_snprintf(buffer, sizeof(buffer), "RAM: %d / %d MB", ram_used, ram_total);
    draw_text(wx + 12, wy + 40, buffer, COLOR_WHITE, COLOR_DARKBLUE);

    // Draw a simple CPU usage bar
    int bar_w = (cpu_usage * (card_w - 30)) / 100;
    fb_fill_rect(wx + 10, wy + 70, bar_w, 10, COLOR_GREEN);
}

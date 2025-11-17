// kernel/kernel.c - main entry
#include "screen.h"
#include "string.h"
#include "idt.h"
#include "interrupts.h"
#include "keyboard.h"
#include "mouse.h"
#include "framebuffer.h"
#include "font.h"
#include "ui.h"
#include "multiboot2.h"
#include "colors.h"
#include "system_info.h"
#include "boot_splash.h"

void kernel_main(void) {
    screen_set_color(COLOR_WHITE, COLOR_BLACK);
    screen_clear();
    screen_write("SynapseOS MVP booting...\n");

    
    
    idt_init();
    interrupts_init();
    keyboard_init();
    mouse_init();
    font_init();

    extern unsigned long multiboot2_magic;
    extern unsigned long multiboot2_info;
    parse_multiboot2(multiboot2_magic, multiboot2_info);

    /* initialize font */
    font_init();
    boot_splash(); 
    /* if framebuffer available, draw advanced UI */
    draw_gui();

    /* main loop: poll keyboard & mouse, update UI */
    while (1) {
        keyboard_poll_and_handle();
        mouse_poll_update();
        system_info_update();
        system_info_draw();
        copy_to_screen();
        /* small delay */
        for (volatile int i=0;i<500000;i++);
    }
}


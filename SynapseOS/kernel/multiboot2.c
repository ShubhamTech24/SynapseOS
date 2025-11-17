#include "framebuffer.h"
#include "screen.h"
#include "string.h"
#include "colors.h"
#include <stdint.h>

void parse_multiboot2(uint32_t magic, uint32_t addr) {
    if (magic != 0x36d76289) {
        kprint("[ !! ] Invalid multiboot2 magic.\n");
        return;
    }

    uint32_t total_size = *(uint32_t*)addr;
    uint32_t *tag = (uint32_t*)(addr + 8);

    while ((uint32_t)tag < addr + total_size) {
        uint32_t type = tag[0];
        uint32_t size = tag[1];

        if (type == 8) { // Framebuffer tag
            uint64_t fb_addr = *((uint64_t*)((uint8_t*)tag + 8));
            uint32_t fb_pitch = *((uint32_t*)((uint8_t*)tag + 16));
            uint32_t fb_width = *((uint32_t*)((uint8_t*)tag + 20));
            uint32_t fb_height = *((uint32_t*)((uint8_t*)tag + 24));
            uint8_t  fb_bpp = *((uint8_t*)((uint8_t*)tag + 28));

            fb_init(fb_width, fb_height, fb_pitch, fb_bpp, (void*)(uintptr_t)fb_addr);
            kprint("[ OK ] Framebuffer ready. Switching to GUI...\n");
            return;
        }

        tag = (uint32_t*)(((uint32_t)tag + size + 7) & ~7);
    }

    kprint("[ !! ] No framebuffer tag found. Using text mode fallback.\n");
}

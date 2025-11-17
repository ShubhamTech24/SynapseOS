#include "colors.h"

/* Smoothly blend two 32-bit colors (ARGB) by factor t (0.0 → 1.0) */
uint32_t lerp_u32(uint32_t a, uint32_t b, float t) {
    uint8_t ar = (a >> 16) & 0xFF;
    uint8_t ag = (a >> 8) & 0xFF;
    uint8_t ab = a & 0xFF;

    uint8_t br = (b >> 16) & 0xFF;
    uint8_t bg = (b >> 8) & 0xFF;
    uint8_t bb = b & 0xFF;

    uint8_t rr = ar + (uint8_t)((br - ar) * t);
    uint8_t rg = ag + (uint8_t)((bg - ag) * t);
    uint8_t rb = ab + (uint8_t)((bb - ab) * t);

    return (0xFF << 24) | (rr << 16) | (rg << 8) | rb;
}

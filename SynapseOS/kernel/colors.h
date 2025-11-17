#ifndef COLORS_H
#define COLORS_H

#include <stdint.h>

/* Common color constants */
#define COLOR_BLACK        0xFF000000
#define COLOR_WHITE        0xFFFFFFFF
#define COLOR_GRAY         0xFF888888
#define COLOR_LIGHTGRAY    0xFFB0B0B0
#define COLOR_DARKGRAY     0xFF202020
#define COLOR_RED          0xFFFF0000
#define COLOR_GREEN        0xFF00FF00
#define COLOR_BLUE         0xFF0000FF
#define COLOR_DARKBLUE     0xFF002040
#define COLOR_CYAN         0xFF00FFFF
#define COLOR_YELLOW       0xFFFFFF00
#define COLOR_LIGHTBLUE    0xFF3399FF
#define COLOR_A            0   
/* Proper declaration only (no inline or static here) */
uint32_t lerp_u32(uint32_t a, uint32_t b, float t);

#endif /* COLORS_H */

#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>

/* Initialize PS/2 mouse (streaming mode). Safe if mouse already inited. */
void mouse_init(void);

/* Poll the mouse (call frequently from main loop). Updates cursor position and draws it. */
void mouse_poll_update(void);

/* Optional: set cursor color (ARGB 32-bit) */
void mouse_set_color(uint32_t color);

#endif /* MOUSE_H */

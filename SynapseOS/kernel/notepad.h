// kernel/notepad.h — header for SynapseOS Notepad App
#ifndef NOTEPAD_H
#define NOTEPAD_H

#include <stdint.h>

#define BUF_CAPACITY 8192

// Functions
void notepad_open(void);
void notepad_close(void);
void notepad_draw(void);
void notepad_handle_key(char c);
void notepad_handle_keycode(uint8_t code);
int  notepad_is_open(void);
const char *notepad_get_text(void);

#endif

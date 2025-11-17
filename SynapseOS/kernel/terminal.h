#pragma once
#include <stddef.h>
#include <stdint.h>

void terminal_initialize(void);
void terminal_setcolor(uint8_t color);
void terminal_putchar(char c);
void terminal_write(const char* data);

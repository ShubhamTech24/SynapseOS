// kernel/string.c
#include "string.h"
#include "screen.h"
#include <stdint.h>

void *memcpy(void *dest, const void *src, size_t n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    for (size_t i = 0; i < n; ++i) d[i] = s[i];
    return dest;
}

void *memset(void *s, int c, size_t n) {
    unsigned char *p = s;
    for (size_t i = 0; i < n; ++i) p[i] = (unsigned char)c;
    return s;
}

void kmemset(void *s, int c, size_t n) {
    memset(s, c, n);
}

size_t strlen(const char *s) {
    size_t i = 0;
    while (s && s[i]) ++i;
    return i;
}

/* high-level prints: kprint -> uses screen text output */
void kprint(const char *s) {
    if (!s) return;
    screen_write(s);
}

void kprint_u32(uint32_t num) {
    char buf[16];
    int idx = 0;
    if (num == 0) { screen_putc('0'); return; }
    while (num > 0 && idx < 15) { buf[idx++] = '0' + (num % 10); num /= 10; }
    for (int i = idx - 1; i >= 0; --i) screen_putc(buf[i]);
}
/* Existing functions (like memcpy, memset, strcmp, strlen) stay above */

/* Add these two implementations: */
char *strcpy(char *dest, const char *src) {
    char *r = dest;
    while ((*dest++ = *src++));
    return r;
}

char *strcat(char *dest, const char *src) {
    char *r = dest;
    while (*dest) dest++;
    while ((*dest++ = *src++));
    return r;
}


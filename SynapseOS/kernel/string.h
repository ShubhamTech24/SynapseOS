#ifndef STRING_H
#define STRING_H
#include <stddef.h>
#include <stdint.h>

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void kmemset(void *s, int c, size_t n);
size_t strlen(const char *s);

void kprint(const char *s);
void kprint_u32(uint32_t num);

int strcmp(const char *s1, const char *s2);
size_t strlen(const char *s);
char *strcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);

#endif


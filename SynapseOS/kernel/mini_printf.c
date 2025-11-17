#include "mini_printf.h"
#include "string.h"
#include <stdarg.h>
#include <stdint.h>

/* Simple integer-to-string (base 10 only) */
static void itoa_simple(int value, char *buf) {
    char tmp[16];
    int i = 0, j = 0;
    int neg = (value < 0);
    if (neg) value = -value;

    do {
        tmp[i++] = (value % 10) + '0';
        value /= 10;
    } while (value);

    if (neg) tmp[i++] = '-';

    while (i--) buf[j++] = tmp[i];
    buf[j] = '\0';
}

/* Very small subset of snprintf: supports %d, %s only */
int mini_snprintf(char *out, unsigned int size, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    unsigned int i = 0;
    for (const char *p = fmt; *p && i < size - 1; p++) {
        if (*p == '%') {
            p++;
            if (*p == 'd') {
                int v = va_arg(args, int);
                char numbuf[32];
                itoa_simple(v, numbuf);
                for (int j = 0; numbuf[j] && i < size - 1; j++)
                    out[i++] = numbuf[j];
            } else if (*p == 's') {
                char *s = va_arg(args, char *);
                while (*s && i < size - 1)
                    out[i++] = *s++;
            } else {
                out[i++] = '%';
                out[i++] = *p;
            }
        } else {
            out[i++] = *p;
        }
    }

    out[i] = '\0';
    va_end(args);
    return i;
}

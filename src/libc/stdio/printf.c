#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int puts(const char *string)
{
    return printf("%s\n", string);
}

static void print(const char *data, size_t data_length)
{
    for ( size_t i = 0; i < data_length; i++ ) {
        putchar((int) ((const unsigned char*) data)[i]);
    }
}

int printf(const char * restrict format, ...)
{
    va_list parameters;
    va_start(parameters, format);

    int written = 0;
    size_t amount;
    bool rejected_bad_specifier = false;

    while ( *format != '\0' ) {
        if ( *format != '%' ) {
print_c:
            amount = 1;
            while ( format[amount] && format[amount] != '%' ) {
                amount++;
            }
            print(format, amount);
            format += amount;
            written += amount;
            continue;
        }

        const char *format_begun_at = format;

        if ( *(++format) == '%' ) {
            goto print_c;
        }

        if ( rejected_bad_specifier ) {
incomprehensible_conversion:
            rejected_bad_specifier = true;
            format = format_begun_at;
            goto print_c;
        }

        char specifier = *format;

        char sbuf[64];
        switch(specifier) {
            case 'c':
                ++format;
                char c = (char) va_arg(parameters, int /* char promotes to int */);
                print(&c, sizeof(c));
                break;
            case 's':
                format++;
                const char *s = va_arg(parameters, const char *);
                print(s, strlen(s));
                break;
            case 'd':
                format++;
                int num = (int) va_arg(parameters, int);
                itoa(num, sbuf, 10);
                print(sbuf, strlen(sbuf));
                break;
            case 'x':
                format++;
                unsigned int hexnum = (unsigned int) va_arg(parameters, unsigned int);
                sbuf[0] = '0';
                sbuf[1] = 'x';
                itoa(hexnum, sbuf+2, 16);
                print(sbuf, strlen(sbuf));
                break;
            default:
                goto incomprehensible_conversion;
        }
    }

    va_end(parameters);

    return written;
}


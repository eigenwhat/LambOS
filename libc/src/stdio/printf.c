#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Function pointer for the print function passed into `vsprintf_internal()`.
 * @param buf The buffer to write to.
 * @param offset (OUT) The offset marking the end of the data in the buffer.
 * @param data The data to write to the buffer.
 * @param data_length The number of bytes to write.
 */
typedef void (*print_func)(char *buf, int *offset, char const *data, size_t data_length);

/* Generic sprintf-like function that takes in a function pointer for handling any "printing". 
 *
 * This lets us separate the parsing logic from the result format, making the actual printf
 * family of functions fairly easy to implement (stub + customized print function). */
static int vsprintf_internal(char *str, char const *format, va_list parameters, print_func print)
{
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
            print(str, &written, format, amount);
            format += amount;
            continue;
        }

        char const *format_begun_at = format;

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
                print(str, &written, &c, sizeof(c));
                break;
            case 's':
                format++;
                char const *s = va_arg(parameters, char const *);
                print(str, &written, s, strlen(s));
                break;
            case 'd':
            case 'i':
                format++;
                int num = (int) va_arg(parameters, int);
                itoa(num, sbuf, 10);
                print(str, &written, sbuf, strlen(sbuf));
                break;
            case 'x':
                format++;
                unsigned int hexnum = (unsigned int) va_arg(parameters, unsigned int);
                sbuf[0] = '0';
                sbuf[1] = 'x';
                itoa(hexnum, sbuf+2, 16);
                print(str, &written, sbuf, strlen(sbuf));
                break;
            default:
                goto incomprehensible_conversion;
        }
    }

    return written;
}

/***** sprintf() *****/

static void sprintf_print(char *buf, int *ptr, char const *data, size_t len)
{
    for (size_t i = 0; i < len; ++i) {
        buf[*ptr] = data[i];
        *ptr += 1;
    }

    /* Set final offset to null terminator. */
    buf[*ptr] = '\0';
}

int vsprintf(char *str, char const * restrict format, va_list parameters)
{
    return vsprintf_internal(str, format, parameters, sprintf_print);
}

int sprintf(char *str, char const * restrict format, ...)
{
    va_list parameters;
    va_start(parameters, format);
    int result = vsprintf(str, format, parameters);
    va_end(parameters);

    return result;
}

/***** printf() *****/

static void printf_print(__attribute__((unused))char *buf, int *ptr, char const *data, size_t data_length)
{
    for ( size_t i = 0; i < data_length; i++ ) {
        putchar((int) ((unsigned char const *) data)[i]);
        *ptr += 1;
    }
}

int puts(char const *string)
{
    return printf("%s\n", string);
}

int vprintf(char const * restrict format, va_list parameters)
{
    return vsprintf_internal(0, format, parameters, printf_print);
}

int printf(char const * restrict format, ...)
{
    va_list parameters;
    va_start(parameters, format);
    int result = vprintf(format, parameters);
    va_end(parameters);

    return result;
}


#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/syscall.h>

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

        enum {
            kLengthDefault,
            kLength_h,
            kLength_hh,
            kLength_l,
            kLength_ll,
            kLength_j,
            kLength_z,
            kLength_t,
            kLength_L
        } length = kLengthDefault;

        const int lengthSpecifierEnd = 2;
        for (int i = 0; i < lengthSpecifierEnd; ++i) {
            // process length sub-specifier
            switch (*format) {
                case 'l':
                    ++format;
                    length = length == kLength_l ? kLength_ll : kLength_l;
                    break;
                case 'h':
                    ++format;
                    length = length == kLength_h ? kLength_hh : kLength_h;
                    break;
                case 'j':
                    ++format;
                    length = kLength_j;
                    i = lengthSpecifierEnd;
                    break;
                case 'z':
                    ++format;
                    length = kLength_z;
                    i = lengthSpecifierEnd;
                    break;
                case 't':
                    ++format;
                    length = kLength_t;
                    i = lengthSpecifierEnd;
                    break;
                case 'L':
                    ++format;
                    length = kLength_L;
                    i = lengthSpecifierEnd;
                    break;
                default:
                    i = lengthSpecifierEnd;
                    break;
            }
        }

        char sbuf[64];
        const char specifier = *format;
        switch(specifier) {
            case 'c':
                ++format;
                char c = (char) va_arg(parameters, int /* char promotes to int */);
                print(str, &written, &c, sizeof(c));
                break;
            case 's':
                ++format;
                char const *s = va_arg(parameters, char const *);
                print(str, &written, s, strlen(s));
                break;
            case 'd':
            case 'i':
                ++format;
                switch (length) {
                    case kLength_ll:
                        lltoa(va_arg(parameters, long long), sbuf, 10);
                        break;
                    case kLength_l:
                    default:
                        itoa(va_arg(parameters, int), sbuf, 10);
                }
                print(str, &written, sbuf, strlen(sbuf));
                break;
            case 'u':
                ++format;
                unsigned int u = (unsigned int) va_arg(parameters, unsigned int);
                uitoa(u, sbuf, 10);
                print(str, &written, sbuf, strlen(sbuf));
                break;
            case 'x':
                ++format;
                unsigned int hexnum = (unsigned int) va_arg(parameters, unsigned int);
                sbuf[0] = '0';
                sbuf[1] = 'x';
                uitoa(hexnum, sbuf+2, 16);
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
    *ptr += sys_write(0, (uint8_t const *)data, data_length);
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

int putchar(int ic)
{
    uint8_t c = (uint8_t)ic;
    sys_write(0, &c, 1);
    return ic;
}

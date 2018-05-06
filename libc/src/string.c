#include <stddef.h>
#include <stdint.h>
#include <string.h>

size_t strlen(char const* str)
{
    size_t ret = 0;
    while ( str[ret] != 0 )
        ret++;
    return ret;
}

char *strcpy(char *dst, char const *src)
{
    char *retval = dst;
    while((*dst++ = *src++));
    return retval;
}

char *strncpy(char *dst, char const *src, size_t num)
{
    char *retval = dst;
    while((num--) && (*dst++ = *src++));
    return retval;
}

int strcmp(char const *str1, char const *str2)
{
    while (*str1 && *str2 && (*str1 == *str2)) { ++str1; ++str2; }
    return *(const unsigned char *)str1 - *(const unsigned char *)str2;
}

int strncmp(char const *str1, char const *str2, size_t num)
{
    while ((num--) && *str1 && *str2 && (*str1++ == *str2++));
    return *(const unsigned char *)str1 - *(const unsigned char *)str2;
}

char *strtok(char * restrict str, char const * restrict delim)
{
    static char *s = NULL;
    if (str != NULL) {
        s = str;
    }

    while (*s) {
        int isDelim = 0;
        for (int i = 0; delim[i] != 0; ++i) {
            if (*s == delim[i]) {
                isDelim = 1;
                break;
            }
        }

        if (!isDelim) {
            break;
        }

        ++s;
    }

    if (*s == 0) {
        return NULL;
    }

    char *tok = s;

    while (*s) {
        for (int i = 0; delim[i] != 0; ++i) {
            if (*s == delim[i]) {
                *s = 0;
                ++s;
                return tok;
            }
        }

        ++s;
    }

    return tok;
}

void *memset(void *ptr, int value, size_t num)
{
    uint8_t *dst = (uint8_t *)ptr;
    for(; num--; *dst++ = (uint8_t)value);

    return ptr;
}

void *memcpy(void *dst, void const *src, size_t num)
{
    return memmove(dst, src, num);
}

void *memmove(void *dst, void const *src, size_t num)
{
    uint8_t *c_dst = (uint8_t *)dst;
    uint8_t const *c_src = (uint8_t const *)src;
    if(dst > src && dst < src + num) {
        c_src += num;
        c_dst += num;
        for(; num--; *--c_dst = *--c_src);
    } else {
        for(; num--; *c_dst++ = *c_src++);
    }

    return dst;
}

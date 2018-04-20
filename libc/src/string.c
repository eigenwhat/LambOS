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

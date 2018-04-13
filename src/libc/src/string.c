#include <stddef.h>
#include <stdint.h>
#include <string.h>

size_t strlen(const char* str)
{
    size_t ret = 0;
    while ( str[ret] != 0 )
        ret++;
    return ret;
}

char *strcpy(char *dst, const char *src)
{
    char *retval = dst;
    while((*dst++ = *src++));
    return retval;
}

char *strncpy(char *dst, const char *src, size_t num)
{
    char *retval = dst;
    while((num--) && (*dst++ = *src++));
    for(; num--; *dst++ = 0);

    return retval;
}

void *memset(void *ptr, int value, size_t num)
{
    uint8_t *dst = (uint8_t *)ptr;
    for(; num--; *dst++ = (uint8_t)value);

    return ptr;
}

void *memcpy(void *dst, const void *src, size_t num)
{
    return memmove(dst, src, num);
}

void *memmove(void *dst, const void *src, size_t num)
{
    uint8_t *c_dst = (uint8_t *)dst;
    const uint8_t *c_src = (const uint8_t *)src;
    if(dst > src && dst < src + num) {
        c_src += num;
        c_dst += num;
        for(; num--; *--c_dst = *--c_src);
    } else {
        for(; num--; *c_dst++ = *c_src++);
    }

    return dst;
}

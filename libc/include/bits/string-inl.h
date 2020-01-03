__LIBC_CONSTEXPR
size_t strlen(char const* str)
{
    size_t ret = 0;
    while ( str[ret] != 0 )
        ret++;
    return ret;
}

__LIBC_CONSTEXPR
char *strcpy(char *dst, char const *src)
{
    char *retval = dst;
    while((*dst++ = *src++));
    return retval;
}

__LIBC_CONSTEXPR
char *strncpy(char *dst, char const *src, size_t num)
{
    char *retval = dst;
    while((num--) && (*dst++ = *src++));
    return retval;
}

__LIBC_CONSTEXPR
int strcmp(char const *str1, char const *str2)
{
    while (*str1 && *str2 && (*str1 == *str2)) { ++str1; ++str2; }
    return *(const unsigned char *)str1 - *(const unsigned char *)str2;
}

__LIBC_CONSTEXPR
int strncmp(char const *str1, char const *str2, size_t num)
{
    while ((num--) && *str1 && *str2 && (*str1++ == *str2++));
    return *(const unsigned char *)str1 - *(const unsigned char *)str2;
}

__LIBC_CONSTEXPR
void *memcpy(void *dst, void const *src, size_t num)
{
    return memmove(dst, src, num);
}

__LIBC_CONSTEXPR
void *memmove(void *dst, void const *src, size_t num)
{
    uint8_t *c_dst = (uint8_t *)dst;
    uint8_t const *c_src = (uint8_t const *)src;
    if(c_dst > c_src && c_dst < c_src + num) {
        c_src += num;
        c_dst += num;
        for(; num--; *--c_dst = *--c_src);
    } else {
        for(; num--; *c_dst++ = *c_src++);
    }

    return dst;
}

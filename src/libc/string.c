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

void *memset(void *ptr, int value, size_t num)
{
	uint8_t *dst = (uint8_t *)ptr;
	while(num > 0) {
		*dst++ = (uint8_t)value;
		--num;
	}

	return ptr;
}

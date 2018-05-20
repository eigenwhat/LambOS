#include <stdio.h>

#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <sys/syscall.h>

int getchar()
{
    uint8_t c;
    sys_read(1, &c, 1);
    return c;
}

char *gets(char *str)
{
    char const *startstr = str;
    while ((*str = (char)getchar()) != '\n') {
        if (*str == '\b') {
            if (str != startstr) {
                --str;
            }
        } else {
            ++str;
        }
    }

    *str = 0;

    return str;
}

//
// Created by Martin Miralles-Cordal on 01/03/2020.
//

#include <string.h>

char *strtok(char * __restrict str, char const * __restrict delim)
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

#include <stdlib.h>

typedef unsigned int uint;

#define XTOA_U_INTERNAL(X_PREFIX) _u##X_PREFIX##toa_internal
#define XTOA_S_INTERNAL(X_PREFIX) _##X_PREFIX##toa_internal

// Defines internal unsigned X handler. (does most of the actual stringifying)
#define XTOA_WRITE_UNSIGNED(X_PREFIX, X_TYPE)                               \
static char *                                                               \
XTOA_U_INTERNAL(X_PREFIX)(unsigned X_TYPE value, char *str, uint base)      \
{                                                                           \
    char *ptr = str;                                                        \
    int length = 0;                                                         \
                                                                            \
    unsigned X_TYPE valcpy = value;                                         \
    do {                                                                    \
        ++length;                                                           \
        valcpy = valcpy / (unsigned X_TYPE)base;                            \
    } while(valcpy != 0);                                                   \
                                                                            \
    ptr[length] = 0;                                                        \
                                                                            \
    do {                                                                    \
        --length;                                                           \
        uint8_t mod = (uint8_t)(value % base);                              \
        if(mod < 10) {                                                      \
            ptr[length] = (char)('0' + mod);                                \
        } else {                                                            \
            ptr[length] = (char)('A' + mod - 10);                           \
        }                                                                   \
        value /= base;                                                      \
    } while(value != 0);                                                    \
                                                                            \
    return str;                                                             \
}

// Defines internal signed X handler (requires matching XTOA_WRITE_UNSIGNED)
#define XTOA_WRITE_SIGNED(X_PREFIX, X_TYPE)                                 \
static char *XTOA_S_INTERNAL(X_PREFIX)(X_TYPE value, char *str, uint base)  \
{                                                                           \
    char *ptr = str;                                                        \
    int i = 0;                                                              \
    if(value < 0) {                                                         \
        ptr[i++] = '-';                                                     \
        value = -value;                                                     \
    }                                                                       \
    XTOA_U_INTERNAL(X_PREFIX)((unsigned X_TYPE)value, str + i, base);       \
    return str;                                                             \
}

// Defines publicly facing xtoa function
#define XTOA_MAIN(X_PREFIX, X_TYPE)                                         \
char *X_PREFIX##toa(X_TYPE value, char *str, int base)                      \
{                                                                           \
    base = CLAMP(base, 2, 32);                                              \
    if(base != 10) {                                                        \
        return XTOA_U_INTERNAL(X_PREFIX)((unsigned X_TYPE)value, str,       \
                                         (uint)base);                       \
    } else {                                                                \
        return XTOA_S_INTERNAL(X_PREFIX)(value, str, (uint)base);           \
    }                                                                       \
}

// Defines publicly facing uxtoa function
#define XTOA_UNSIGNED_MAIN(X_PREFIX, X_TYPE)                                \
char *u##X_PREFIX##toa(unsigned X_TYPE value, char *str, int base)          \
{                                                                           \
    base = CLAMP(base, 2, 32);                                              \
    return XTOA_U_INTERNAL(X_PREFIX)(value, str, (uint)base);               \
}

#define XTOA(X_PREFIX, X_TYPE) \
XTOA_WRITE_UNSIGNED(X_PREFIX, X_TYPE) \
XTOA_WRITE_SIGNED(X_PREFIX, X_TYPE) \
XTOA_MAIN(X_PREFIX, X_TYPE)

// Signed type public functions
XTOA(i, int)
XTOA(l, long)
XTOA(ll, long long)

// Unsigned type public functions
XTOA_UNSIGNED_MAIN(i, int)
XTOA_UNSIGNED_MAIN(l, long)
XTOA_UNSIGNED_MAIN(ll, long long)

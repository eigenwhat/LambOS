#include <stdlib.h>

char *_itoa_unsigned(unsigned int value, char *str, int base)
{
    char *ptr = str;
    int length = 0;

    unsigned int valcpy = value;

    do {
        ++length;
        valcpy /= base;
    } while(valcpy != 0);

    ptr[length] = 0;
    do {
        --length;
        uint8_t mod = value % base;
        if(mod < 10) {
            ptr[length] = '0' + mod;
        } else {
            ptr[length] = 'A' + mod - 10;
        }
        value /= base;
    } while(value != 0);

    return str;
}

char *_itoa_signed(int value, char *str, int base)
{
    char *ptr = str;
    int i = 0;
    if(value < 0) {
        ptr[i++] = '-';
        value = -value;
    }

    _itoa_unsigned((unsigned int)value, str + i, base);

    return str;
}

char *itoa(int value, char *str, int base) {
    base = CLAMP(base, 2, 32);
    if(base != 10) {
        return _itoa_unsigned((unsigned int)value, str, base);
    } else {
        return _itoa_signed(value, str, base);
    }
}

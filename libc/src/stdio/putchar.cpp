#include <stdio.h>
#include <sys/syscall.h>

extern "C" {

int putchar(int ic)
{
    char c = (char) ic;

    uint8_t const *buf = reinterpret_cast<uint8_t const *>(&c);

    sys_write(0, buf, 1);

    return ic;
}

} // extern "C"
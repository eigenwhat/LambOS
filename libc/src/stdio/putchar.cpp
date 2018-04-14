#include <Kernel.hpp>
#include <stdio.h>

extern "C" {

int putchar(int ic)
{
    char c = (char) ic;
    kernel->out()->print(c);

    return ic;
}

} // extern "C"

#include <stdio.h>

#ifdef KERNEL
#include <Kernel.hpp>
#endif

extern "C" {

int putchar(int ic)
{
#ifdef KERNEL
    char c = (char) ic;
    kernel->out()->print(c);
#endif
    return ic;
}

} // extern "C"

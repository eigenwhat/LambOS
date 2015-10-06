#include <Kernel.hpp>
#include <stdio.h>

int putchar(int ic)
{
#if defined(__is_myos_kernel)
    char c = (char) ic;
    kernel->out()->print(c);
#else
    // TODO: You need to implement a write system call.
#endif
    return ic;
}


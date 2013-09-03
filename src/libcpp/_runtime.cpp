#include <Kernel.hpp>

extern "C" {

void __cxa_pure_virtual()
{
    // Do nothing or print an error message.
}

void * __stack_chk_guard = NULL;
 
void __stack_chk_guard_setup()
{
    unsigned char * p;
    p = (unsigned char *) &__stack_chk_guard;

    /* If you have the ability to generate random numbers in your kernel then use them,
       otherwise for 32-bit code: */
    p[0] = 0;
    p[1] = 0;
    p[2] = '\n';
    p[3] = 0xFF;
}
 
void __attribute__((noreturn)) __stack_chk_fail()
{ 
    kernel->panic("Stack buffer overflow detected!");
    for(;;);
}

}

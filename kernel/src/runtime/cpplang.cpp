#include <Kernel.hpp>

extern "C" {

void __cxa_pure_virtual()
{
    // Do nothing or print an error message.
}

void * __stack_chk_guard = nullptr;
 
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

namespace __cxxabiv1 {
//see: https://wiki.osdev.org/C%2B%2B#Local_Static_Variables_.28GCC_Only.29

/* guard variables */

/* The ABI requires a 64-bit type.  */
__extension__ typedef int __guard __attribute__((mode(__DI__)));

int __cxa_guard_acquire(__guard *g) { return !*(char *)(g); }

void __cxa_guard_release(__guard *g) { *(char *)g = 1; }

void __cxa_guard_abort(__guard *) {}

} // namespace __cxxabiv1


} // extern "C"

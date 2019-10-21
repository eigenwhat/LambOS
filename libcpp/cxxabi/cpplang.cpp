//
// Created by Martin Miralles-Cordal on 9/2/2013.
//

extern "C" {

void __cxa_pure_virtual() { /* Do nothing or print an error message. */ }

//see: https://wiki.osdev.org/C%2B%2B#Local_Static_Variables_.28GCC_Only.29

/* guard variables */

/* The ABI requires a 64-bit type.  */
__extension__ typedef int __guard __attribute__((mode(__DI__)));
int __cxa_guard_acquire(__guard *g) { return !*(char *)(g); }
void __cxa_guard_release(__guard *g) { *(char *)g = 1; }
void __cxa_guard_abort(__guard *) {}

} // extern "C"

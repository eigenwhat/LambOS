#include <stddef.h>
#include <stdint.h>
#include "VGA4BitColor.h"
#include "BasicTerminal.h"
 
/* Check if the compiler thinks if we are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

extern "C" void __cxa_pure_virtual()
{
    // Do nothing or print an error message.
}

extern "C" void kernel_main()
{
	BasicTerminal terminal;
	/* Since there is no support for newlines in terminal_putchar yet, \n will
	   produce some VGA specific character instead. This is normal. */
	terminal.writeString("Hello, kernel World!\n");
	terminal.setForegroundColor(COLOR_LIGHT_BLUE);
	terminal.writeString("New color!");
}


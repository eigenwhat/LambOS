#include <Kernel.hpp>
#include <string.h>

void testMalloc()
{
    const char *somestr = "This is a test string to see if kmalloc works now.";
    char *longstr = (char*)kmalloc(strlen(somestr) + 1);
    if(log_test("Testing if kmalloc gives us an appropriate pointer...", (uint32_t)longstr > (uint32_t)&kernel_end)) {
        strcpy(longstr, somestr);
        kernel->out()->println(longstr);
    } else {
        kernel->panic("kmalloc no work :(");
    }
}

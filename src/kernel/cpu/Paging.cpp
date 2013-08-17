#include "Paging.hpp"

void PageDirectory::install()
{
    asm("movl %1, %0;"
        "movl %0, %%cr3;"
        "movl %%cr0, %0;"
        "orl 0x80000000, %0;"
        "movl %0, %%cr0" : : "r"(0), "r"(_pageDirectoryAddress));
}
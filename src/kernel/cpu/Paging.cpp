#include "Paging.hpp"

//======================================================
// PageTable
//======================================================
PageTable::PageTable(uint32_t *tableAddress) : _tableAddress(tableAddress) 
{
    for(int i = 0; i < 1024; i++)
    {
        _tableAddress[i] = 0 | 2; 
    }
}

void PageTable::install()
{
    asm("movl %1, %0;"
        "movl %0, %%cr3;"
        "movl %%cr0, %0;"
        "orl 0x80000000, %0;"
        "movl %0, %%cr0" : : "r"(0), "r"(_tableAddress));
}
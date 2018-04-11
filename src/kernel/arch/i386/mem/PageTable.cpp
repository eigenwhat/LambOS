#include <mem/PageTable.hpp>
#include <sys/asm.h>
#include <stdio.h>

//======================================================
// PageTable
//======================================================
void PageTable::clear()
{
    for (int i = 0; i < 1024; i++) {
        _tableAddress[i] = 0;
    }
}

PageEntry PageTable::entryAtIndex(uint16_t index)
{
    PageEntry entry(_tableAddress[index]);
    entry.setFlags(_tableAddress[index]);
    return entry;
}

void PageTable::setEntry(uint16_t index, PageEntry entry)
{
    _tableAddress[index] = entry._entry;
    invlpg(entry.address());
}

void PageTable::install()
{
    asm("movl %1, %0;"
        "movl %0, %%cr3;"
        "movl %%cr0, %0;"
        "orl 0x80000001, %0;"
        "movl %0, %%cr0" : : "r"(0), "r"(_tableAddress));
}

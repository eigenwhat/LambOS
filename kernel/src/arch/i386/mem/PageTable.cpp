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
    // TODO: Figure out why using %eax here instead of %0 is so important as to otherwise break paging on QEMU
    asm("movl %0, %%cr3\n"
        "mov %%cr0, %%eax\n"
        "orl $0x80000000, %%eax\n"
        "mov %%eax, %%cr0\n" :: "r"(_tableAddress) : "%eax");
}

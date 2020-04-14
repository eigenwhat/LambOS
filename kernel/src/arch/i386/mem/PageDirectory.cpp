//
// Created by Martin Miralles-Cordal on 04/09/2020.
//

#include <arch/i386/mem/Paging.hpp>

namespace X86 {

void PageDirectory::install()
{
    asm volatile ("movl %0, %%cr3\n"
                  "mov %%cr0, %0\n"
                  "orl $0x80000000, %0\n"
                  "mov %0, %%cr0\n" :: "r"(physicalAddress_));
}

void PageDirectory::clear()
{
//    for (auto &table : tables_) { table.clear(); }
    tableFrames_.fill(0);
}

} // namespace X86
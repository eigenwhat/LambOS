//
// Created by Martin Miralles-Cordal on 8/17/2013.
//

#include <mem/PageTable.hpp>

#include <arch/i386/mem/Paging.hpp>
#include <system/asm.h>

#include <cstring>

//======================================================
// PageTable
//======================================================

void PageTable::invalidatePage(uint32_t m) { invlpg(m); }
void PageTable::clear() { std::memset(_tableAddress, 0, X86::kPageTableSize); }

void PageTable::install()
{
    // TODO: Figure out why using %eax here instead of %0 is so important as to otherwise break paging on QEMU
    asm("movl %0, %%cr3\n"
        "mov %%cr0, %%eax\n"
        "orl $0x80000000, %%eax\n"
        "mov %%eax, %%cr0\n" :: "r"(_tableAddress) : "%eax");
}

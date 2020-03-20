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

void PageTable::invalidatePage(uintptr_t m) { invlpg(m); }
void PageTable::clear() { std::memset(_tableAddress, 0, X86::kPageTableSize); }

void PageTable::install()
{
    asm volatile ("movl %0, %%cr3\n"
                  "mov %%cr0, %0\n"
                  "orl $0x80000000, %0\n"
                  "mov %0, %%cr0\n" :: "r"(_tableAddress));
}

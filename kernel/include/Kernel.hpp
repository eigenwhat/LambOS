//
// Created by Martin Miralles-Cordal on 8/14/2013.
//

#pragma once

#include <cstddef>
#include <cstdint>
#include <proc/Context.hpp>
#include <arch/i386/mem/MMU.hpp>
#include <cpu/CPU.hpp>

class Kernel : public Context
{
  public:
    /**
     * Kills the system.
     *
     * @param errorMessage A descriptive message regarding the error that has occurred, or at least
     *                     some words of comfort.
     */
    void panic(char const *errorMessage);

    /**
     * Allocates contiguous pages of memory.
     * @param numberOfPages The number of pages to allocate.
     * @return the start of the contiguous allocated memory.
     */
    void *palloc(size_t numberOfPages) { return _mmu->palloc(addressSpace(), numberOfPages); }

    /**
     * Attempts to allocate a number of pages at the given address.
     * @param virtualAddress
     * @param numberOfPages The number of pages to allocate.
     * @return the start of the contiguous allocated memory.
     */
    void *palloc(void *virtualAddress, size_t numberOfPages)
    {
        return _mmu->palloc(addressSpace(), virtualAddress, numberOfPages);
    }

    /**
     * Frees a page-aligned block of memory.
     * @param startOfMemoryRange The start of the memory block to free.
     * @param numberOfPages The length of the memory block in pages. Defaults to 1.
     * @return -1 if an error occurred, 0 otherwise.
     */
    int pfree(void *startOfMemoryRange, size_t numberOfPages = 1)
    {
        return _mmu->pfree(addressSpace(), startOfMemoryRange, numberOfPages);
    }

  protected:
    Kernel() = default;

    MMU *_mmu = nullptr;
};

extern Kernel *kernel;

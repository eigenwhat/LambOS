//
// Created by Martin Miralles-Cordal on 8/28/2013.
//

#pragma once

#include <mem/Address.hpp>
#include <mem/AddressSpace.hpp>
#include <mem/PageFrameAllocator.hpp>

namespace X86 {


/** Abstraction for the X86 memory management unit. Implements paging. */
class MMU
{
  public:
    MMU(uint32_t mmap_addr, uint32_t mmap_length);

    /**
     * Allocates contiguous pages of memory.
     * @param addressSpace The address space to allocate within.
     * @param numberOfPages The number of pages to allocate.
     * @return the start of the contiguous allocated memory.
     */
    void *palloc(AddressSpace addressSpace, size_t numberOfPages);

    /**
     * Attempts to allocate a number of pages at the given address.
     * @param addressSpace The address space to allocate within.
     * @param virtualAddress The target address.
     * @param numberOfPages The number of pages to allocate.
     * @return the start of the contiguous allocated memory.
     */
    void *palloc(AddressSpace addressSpace, VirtualAddress virtualAddress, size_t numberOfPages);

    /**
     * Frees a page-aligned block of memory.
     * @param addressSpace The address space to free from.
     * @param startOfMemoryRange The start of the memory block to free.
     * @param numberOfPages The length of the memory block in pages. Defaults to 1.
     * @return -1 if an error occurred, 0 otherwise.
     */
    int pfree(AddressSpace addressSpace, void *startOfMemoryRange, size_t numberOfPages = 1);

    PageTable cloneDirectory(AddressSpace src);

    AddressSpace create();

    /** Prepares and installs the kernel bootstrap address space. */
    void install(AddressSpace addressSpace);

    /** Drops temporary low identity mappings used during kernel bootstrap. */
    bool dropKernelBootstrapIdentityMappings();

  private:
    PageTable getOrCreateTable(AddressSpace addressSpace, uint16_t directoryIndex);
    PageTable tableForAddress(AddressSpace addressSpace, void *virtualAddress);
    PageEntry pageForAddress(AddressSpace addressSpace, void *virtualAddress);
    void allocatePages(AddressSpace addressSpace, VirtualAddress address, size_t numberOfPages);
    void _flush();

    PageFrameAllocator _pageFrameAllocator;
    bool _pagingEnabled = false;
    uint16_t _identityLastDirectoryIndex = 0;
    bool _identityMappingsDropped = false;
};

} // namespace X86

using X86::MMU;

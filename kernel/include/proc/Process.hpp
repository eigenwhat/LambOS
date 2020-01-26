//
// Created by Martin Miralles-Cordal on 12/22/2019.
//

#pragma once

#include <mem/PageTable.hpp>
#include <util/LinkedList.hpp>
#include <util/Maybe.hpp>
#include <util/String.hpp>

using FileHandle = std::size_t;

struct Process
{
    // architecture-specific process state
    struct ICpuState { virtual ~ICpuState() = default; };

    // Per-process state
    enum class State { Unused, Embryo, Sleeping, Runnable, Running, Zombie };
    using ID = int;

    std::uint32_t sz;                      // Size of process memory (bytes)
    PageTable *pgdir;                      // Page table
    std::byte *kernStack;                  // Bottom of kernel stack for this process
    State state;                           // Process state
    ID pid;                                // Process ID
    Process *parent;                       // Parent process
    ICpuState *procState;                  // architecture-dependent state (trap frame, context, channel)
    bool killed;                           // If non-zero, have been killed
    sys::LinkedList<FileHandle> openFiles; // Open files
    FileHandle cwd;                        // Current directory
    sys::String name;                      // Process name (debugging)
};

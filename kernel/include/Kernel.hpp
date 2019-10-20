//
// Created by Martin Miralles-Cordal on 8/14/2013.
//

#pragma once

#include <cstddef>
#include <cstdint>
#include <proc/Context.hpp>
#include <mem/MMU.hpp>
#include <cpu/CPU.hpp>

class Kernel : public Context
{
  public:
    /** Returns the kernel's representation of the memory management unit. */
    MMU *mmu() { return this->_mmu; }

    /**
     * Kills the system.
     *
     * @param errorMessage A descriptive message regarding the error that has occurred, or at least
     *                     some words of comfort.
     */
    void panic(char const *errorMessage);

  protected:
    Kernel() = default;
    void setMMU(MMU *mmu) { _mmu = mmu; }

  private:
    MMU *_mmu = nullptr;
};

extern Kernel *kernel;

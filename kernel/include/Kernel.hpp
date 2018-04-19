#pragma once

#include <stddef.h>
#include <stdint.h>
#include <proc/Context.hpp>
#include <mem/MMU.hpp>
#include <cpu/CPU.hpp>

class Kernel : public Context
{
  public:
    /** Returns the kernel's representation of the memory management unit. */
    MMU *mmu() { return this->_mmu; }

    /** Returns the kernel's representation of the CPU. */
    CPU *cpu() { return this->_cpu; }

    /**
     * Kills the system.
     *
     * @param errorMessage A descriptive message regarding the error that has occurred, or at least
     *                     some words of comfort.
     */
    void panic(char const *errorMessage);

  protected:
    Kernel(CPU *cpu, MMU *mmu) : _cpu(cpu), _mmu(mmu) {}
    void setCPU(CPU *cpu) { _cpu = cpu; }
    void setMMU(MMU *mmu) { _mmu = mmu; }

  private:
    CPU *_cpu = nullptr;
    MMU *_mmu = nullptr;
};

extern Kernel *kernel;

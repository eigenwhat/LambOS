#pragma once

#include <stddef.h>
#include <stdint.h>
#include <proc/Context.hpp>
#include <mem/MMU.hpp>
#include <cpu/CPU.hpp>

class Kernel : public virtual Context
{
public:
    Kernel() : _cpu(NULL), _mmu(NULL) {}

    // accessors/mutators
    void setMMU(MMU *mmu) { _mmu = mmu; }
    MMU *mmu() { return this->_mmu; }

    void setCPU(CPU *cpu) { _cpu = cpu; }
    CPU *cpu() { return this->_cpu; }

    // methods
    void panic(const char *errorMessage);

private:
    CPU *_cpu;
    MMU *_mmu;
};

extern Kernel *kernel;

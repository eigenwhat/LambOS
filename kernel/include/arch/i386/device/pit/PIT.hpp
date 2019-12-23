//
// Created by Martin Miralles-Cordal on 12/22/2019.
//

#pragma once

#include <arch/i386/device/pit/PITIRQ.hpp>

extern "C" uint16_t read_PIT_count();

class PIT
{
  public:
    static void installIRQ(X86 &cpu) { PITIRQ::install(cpu); }

    static uint16_t readPitCount()
    {
        return read_PIT_count();
    }

  private:
};
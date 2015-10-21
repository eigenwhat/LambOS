#pragma once

#include <stddef.h>
#include <stdint.h>
#include <cpu/RealTimeClock.hpp>

class X86RealTimeClock : RealTimeClock {
public:
    virtual DateTime currentTime();
};

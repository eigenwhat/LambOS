#pragma once

#include <cstddef>
#include <stdint.h>
#include <cpu/RealTimeClock.hpp>

class X86RealTimeClock : public RealTimeClock
{
public:
    DateTime currentTime() override;
};

//
// Created by Martin Miralles-Cordal on 10/20/2015.
//

#pragma once

#include <cstddef>
#include <stdint.h>

struct DateTime
{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t weekday;
    uint8_t monthday;
    uint8_t month;
    uint8_t year;
    uint8_t century;
};


namespace concepts {

template <typename T>
concept RealTimeClock = requires(T a) {
    {a.currentTime()} -> DateTime;
};

} // namespace concepts

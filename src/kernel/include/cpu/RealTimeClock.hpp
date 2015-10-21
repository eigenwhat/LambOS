#pragma once

#include <stddef.h>
#include <stdint.h>

struct DateTime {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t weekday;
    uint8_t monthday;
    uint8_t month;
    uint8_t year;
    uint8_t century;
};

class RealTimeClock {
public:
    virtual DateTime currentTime() = 0;
};


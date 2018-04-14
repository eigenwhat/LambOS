#include <arch/i386/cpu/X86RealTimeClock.hpp>

#include <sys/asm.h>

#define kCMOSSetRegister 0x70
#define kCMOSReadAddress 0x71

enum
{
    cmos_address = 0x70,
    cmos_data = 0x71
};

uint8_t get_rtc_register(int reg)
{
    outb(kCMOSSetRegister, reg);
    return inb(kCMOSReadAddress);
}

uint8_t get_update_in_progress_flag()
{
    return get_rtc_register(0x0A) & 0x80;
}

uint8_t convert_bcd_if_needed(uint8_t value)
{
    if (!(get_rtc_register(0x0B) & 0x04)) {
        value = (value & 0x0F) + ((value / 16) * 10);
    }

    return value;
}

//=============================================
// X86RealTimeClock Implementation
//=============================================

DateTime X86RealTimeClock::currentTime()
{
    DateTime currentTime;
    while (get_update_in_progress_flag());
    currentTime.seconds = convert_bcd_if_needed(get_rtc_register(0x00));
    currentTime.minutes = convert_bcd_if_needed(get_rtc_register(0x02));
    currentTime.hours = convert_bcd_if_needed(get_rtc_register(0x04));
    currentTime.weekday = convert_bcd_if_needed(get_rtc_register(0x06));
    currentTime.monthday = convert_bcd_if_needed(get_rtc_register(0x07));
    currentTime.month = convert_bcd_if_needed(get_rtc_register(0x08));
    currentTime.year = convert_bcd_if_needed(get_rtc_register(0x09));
    currentTime.century = convert_bcd_if_needed(get_rtc_register(0x32));

    return currentTime;
}


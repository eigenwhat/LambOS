#include <arch/i386/cpu/X86RealTimeClock.hpp>

#include <sys/asm.h>


namespace {

constexpr uint8_t kCMOSSetRegister = 0x70;
constexpr uint8_t kCMOSReadAddress = 0x71;

enum class RtcRegister {
    Seconds     = 0x00,
    Minutes     = 0x02,
    Hours       = 0x04,
    Weekday     = 0x06,
    Monthday    = 0x07,
    Month       = 0x08,
    Year        = 0x09,
    Century     = 0x32,
    StatusA     = 0x0A,
    StatusB     = 0x0B
};

uint8_t readRtcRegister(RtcRegister reg)
{
    outb(kCMOSSetRegister, static_cast<uint8_t>(reg));
    return inb(kCMOSReadAddress);
}

uint8_t get_update_in_progress_flag()
{
    return readRtcRegister(RtcRegister::StatusA) & static_cast<uint8_t>(0x80);
}

uint8_t convert_bcd_if_needed(uint8_t value)
{
    if (!(readRtcRegister(RtcRegister::StatusB) & 0x04)) {
        value = (value & 0x0F) + ((value / 16) * 10);
    }

    return value;
}

}

//=============================================
// X86RealTimeClock Implementation
//=============================================

DateTime X86RealTimeClock::currentTime()
{
    DateTime currentTime;
    while (get_update_in_progress_flag());
    currentTime.seconds = convert_bcd_if_needed(readRtcRegister(RtcRegister::Seconds));
    currentTime.minutes = convert_bcd_if_needed(readRtcRegister(RtcRegister::Minutes));
    currentTime.hours = convert_bcd_if_needed(readRtcRegister(RtcRegister::Hours));
    currentTime.weekday = convert_bcd_if_needed(readRtcRegister(RtcRegister::Weekday));
    currentTime.monthday = convert_bcd_if_needed(readRtcRegister(RtcRegister::Monthday));
    currentTime.month = convert_bcd_if_needed(readRtcRegister(RtcRegister::Month));
    currentTime.year = convert_bcd_if_needed(readRtcRegister(RtcRegister::Year));
    currentTime.century = convert_bcd_if_needed(readRtcRegister(RtcRegister::Century));

    return currentTime;
}


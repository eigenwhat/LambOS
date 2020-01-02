#pragma once

#include <Memory.hpp>

/** Abstract interface for a device on the machine. */
class Device
{
  public:
    virtual ~Device() = default;

    /**
     * The name of the device as reported by the device.
     * @return A C string with the device's name.
     */
    virtual char const * model() = 0;

    /**
     * The serial code of the device reported by the device.
     * @return A C string with the device's serial.
     */
    virtual char const * serial() = 0;

    /**
     * The firmware version of the device as reported by the device.
     * @return A C string with the device's firmware version.
     */
    virtual char const * firmware() = 0;
};

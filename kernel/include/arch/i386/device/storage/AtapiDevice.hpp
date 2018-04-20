#pragma once

#include <arch/i386/device/storage/AtaDevice.hpp>

#include <stdint.h>
#include <cstring>

class AtapiDevice : public AtaDevice
{
  public:
    AtapiDevice(bool primary, bool master) : AtaDevice(primary, master) {}

    /**
     * The name of the device as reported by the device.
     * @return A C string with the device's name.
     */
    char const *model() override;

    /**
     * The serial code of the device reported by the device.
     * @return A C string with the device's serial.
     */
    char const *serial() override;

    /**
     * The firmware version of the device as reported by the device.
     * @return A C string with the device's firmware version.
     */
    char const *firmware() override;

  private:
    void identify();
};

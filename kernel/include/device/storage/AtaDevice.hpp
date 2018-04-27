#pragma once

#include <device/Device.hpp>

#include <stddef.h>
#include <stdint.h>

/** Representation of an ATA or ATAPI device. */
class AtaDevice : public Device
{
  public:
    enum class Type {
        PATA,
        SATA,
        PATAPI,
        SATAPI,
        Unknown
    };

    /**
     * Returns whether the device is primary or secondary. Relevant only for P-ATA/P-ATAPI.
     * @return true if primary, false otherwise.
     */
    virtual bool isPrimary() const = 0;

    /**
     * Returns whether the device is a master or slave. Relevant only for P-ATA/P-ATAPI.
     * @return True if master, false if slave.
     */
    virtual bool isMaster() const = 0;

    /**
     * Returns whether or not there is a device attached in this slot.
     * @return True if a device is there, false otherwise.
     */
    virtual bool isAttached() const = 0;

    /**
     * The name of the device as reported by the device.
     * @return A C string with the device's name.
     */
    virtual char const *model() = 0;

    /**
     * The serial code of the device reported by the device.
     * @return A C string with the device's serial.
     */
    virtual char const *serial() = 0;

    /**
     * The firmware version of the device as reported by the device.
     * @return A C string with the device's firmware version.
     */
    virtual char const *firmware() = 0;

    /**
     * The size of the device's sectors.
     * @return An unsigned 32-bit integer with the sector size in bytes.
     */
    virtual uint32_t sectorSize() const = 0;

    /**
     * The type of device. [P-ATA, P-ATAPI, S-ATA, S-ATAPI, unknown]
     * @return The Type enum value for this device.
     */
    virtual Type type() const = 0;

    /**
     * Reads in a sector.
     * @param address The linear block address of the sector to read.
     * @param buf The buffer to store the fetched data in. It should be at least
     *            the size of the device sector.
     * @param sectors The number of sectors to read. Defaults to 1.
     * @return true if successful, false otherwise.
     */
    virtual bool read(uint64_t address, uint16_t *buf, size_t sectors = 1) = 0;
};
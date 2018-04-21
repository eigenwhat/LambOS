#pragma once

#include <Object.hpp>

class AtaDeviceDescriptor : public Object
{
  public:
    /**
     * @param rawInfo The 512 byte (uint16_t[256]) identity information as received from the IO bus.
     */
    AtaDeviceDescriptor(uint16_t *rawInfo = nullptr) { readIdentity(rawInfo); }

    /**
     * @param rawInfo The 512 byte (uint16_t[256]) identity information as received from the IO bus.
     */
    void readIdentity(uint16_t *rawInfo);

    /**
     * Reads in the data gathered from the ATAPI READ CAPACITY command.
     * @param rawInfo The 8 byte (uint16_t[4]) capacity information as received from the IO bus.
     */
    void readAtapiCapacity(uint16_t *rawInfo);

    /**
     * The serial code of the device reported by the device.
     * @return A C string with the device's serial.
     */
    char const *serial() const { return _serial; }

    /**
     * The firmware version of the device as reported by the device.
     * @return A C string with the device's firmware version.
     */
    char const *firmware() const { return _firmware; }

    /**
     * The model of the device as reported by the device.
     * @return A C string with the device's model.
     */
    char const *model() const { return _model; }

    /** The number of 28-bit LBA addressable sectors. */
    uint32_t lba28bitSectorCount() const { return _lba28bitSectorCount; }

    /** The number of 48-bit LBA addressable sectors. */
    uint64_t lba48bitSectorCount() const { return _lba48bitSectorCount; }

    /** The last valid linear block address for the ATAPI device. */
    uint32_t atapiEndLba() const { return _atapiEndLba; }

    /** The sector size for the device. */
    uint32_t sectorSize() const { return _sectorSize; }

  private:
    char _firmware[9];
    char _model[41];
    char _serial[21];
    uint32_t _lba28bitSectorCount = 0;
    uint64_t _lba48bitSectorCount = 0;
    uint32_t _atapiEndLba = 0;
    uint32_t _sectorSize = 0;
};

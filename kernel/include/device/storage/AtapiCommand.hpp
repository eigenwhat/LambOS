#pragma once

#include <stdint.h>
#include <cstring>

class AtapiCommand
{
  public:
    union Packet
    {
        /**
         * The packet as bytes. Used for setting the values.
         *
         * Bytes:
         * 00 - SCSI command opcode
         * 01 - reserved
         * 02 - LBA begin (big endian)
         * 03 - LBA 2
         * 04 - LBA 3
         * 05 - LBA end
         * 06 - transfer length begin (big endian)
         * 07 - transfer length 2
         * 08 - transfer length 3 / end (for certain commands)
         * 09 - transfer length end / flag bits (for certain commands)
         * 10 - miscellaneous
         * 11 - reserved
         */
        uint8_t bytes[12];

        /**
         * The packet as words. Used for sending to device.
         */
        uint16_t words[6];
    } __attribute__((packed));

    /**
     * The READ CAPACITY command fetches the sector size and last reachable LBA.
     * @return The command object, ready for use.
     */
    static AtapiCommand readCapacityCommand();

    /**
     * The READ (12) command loads a sector or sectors into memory.
     * @param lba The linear block address of the sector to read.
     * @param length The number of sectors to read. Defaults to 1.
     * @return The command object, ready for use.
     */
    static AtapiCommand read12Command(uint32_t lba, uint32_t length = 1);

    /**
     * The packet corresponding to the command to be sent to the device.
     * @return A union of byte and word arrays 12 bytes long.
     */
    const Packet &packet() const { return _packet; }
    Packet &packet() { return _packet; }

  private:
    Packet _packet;
};

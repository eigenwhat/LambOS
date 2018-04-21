#pragma once

#include <stdint.h>
#include <cstring>

class AtapiCommand
{
  public:
    union Packet {
        uint8_t bytes[12];
        uint16_t words[6];
    };

    /**
     * The READ CAPACITY command fetches the sector size and last reachable LBA.
     * @return The command object, ready for use.
     */
    static AtapiCommand readCapacityCommand();

    /**
     * The packet corresponding to the command to be sent to the device.
     * @return A union of byte and word arrays 12 bytes long.
     */
    const Packet &packet() const { return _packet; }
    Packet &packet() { return _packet; }

  private:
    Packet _packet;
};

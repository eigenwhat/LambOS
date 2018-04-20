#include <arch/i386/device/storage/AtapiDevice.hpp>
#include <sys/asm.h>
#include <cstdio>
#include <cstring>

namespace {

void fixAtaWordString(char *str, size_t len)
{
    // ATAPI info is read one 16-bit word at a time, which means strings got scrambled by little-endian
    // (e.g. str is 214365 instead of 123456)
    for (size_t i = 0; i < len; i += 2) {
        uint8_t tmp = str[i + 1];
        str[i + 1] = str[i];
        str[i] = tmp;
    }
}

typedef struct
{
    uint16_t flags;
    uint16_t unused1[9];
    char serial[20];
    uint16_t unused2[3];
    char firmware[8];
    char model[40];
    uint16_t sectors_per_int;
    uint16_t unused3;
    uint16_t capabilities[2];
    uint16_t unused4[2];
    uint16_t valid_ext_data;
    uint16_t unused5[5];
    uint16_t size_of_rw_mult;
    uint32_t sectors_28;
    uint16_t unused6[38];
    uint64_t sectors_48;
    uint16_t unused7[152];
} __attribute__((packed)) ata_identify_t;

class AtapiCommand
{
  public:
    union Packet {
        uint8_t command_bytes[12];
        uint16_t command_words[6];
    };

    Packet &packet() { return _packet; }

  private:
    Packet _packet;
};

}

char const *AtapiDevice::model()
{
    if (!_identified) {
        identify();
        _identified = true;
    }

    return _model;
}

char const *AtapiDevice::serial()
{
    if (!_identified) {
        identify();
        _identified = true;
    }

    return _serial;
}

char const *AtapiDevice::firmware()
{
    if (!_identified) {
        identify();
        _identified = true;
    }

    return _firmware;
}

void AtapiDevice::identify()
{
    // Set features register. 0 = PIO, 1 = DMA.
    outb(ioPort() + 1, 0);
    outb(controlPort(), 0);

    // select device
    outb(ioPort() + kAtaRegisterDriveSelect, 0xA0 | slaveBit() << 4);
    ioWait();

    // send IDENTIFY PACKET DEVICE
    outb(ioPort() + 0x07, 0xA1);
    ioWait();

    // read in status
    waitUntilReady(false);

    // read in device information
    uint16_t identity[256];
    ata_identify_t *idstruct = (ata_identify_t *) identity;
    for (int i = 0; i < 256; ++i) {
        identity[i] = inw(ioPort());
    }

    // hello endianness, my old friend...
    fixAtaWordString(idstruct->serial, 20);
    fixAtaWordString(idstruct->firmware, 8);
    fixAtaWordString(idstruct->model, 40);

    // transfer info to permanent, sane home
    strncpy(_serial, idstruct->serial, 20);
    strncpy(_firmware, idstruct->firmware, 8);
    strncpy(_model, idstruct->model, 40);
}

#include <arch/i386/device/storage/AtaDevice.hpp>
#include <sys/asm.h>
#include <cstdio>

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

AtaDevice::Type AtaDevice::typeOf(bool primary, bool master)
{
    // stack initialization is cheap, so IDGAF
    return AtaDevice(primary, master).type();
}

bool AtaDevice::isAttached() const
{
    softReset(); /* waits until master drive is ready again */
    outb(_ioPort + kAtaRegisterDriveSelect, 0xA0 | _slaveBit << 4);
    outb(_ioPort + kAtaRegisterLbaLow, 0x0);
    outb(_ioPort + kAtaRegisterLbaMid, 0x0);
    outb(_ioPort + kAtaRegisterLbaHigh, 0x0);
    outb(_ioPort + kAtaRegisterCommand, 0xEC);
    return waitForStatus(-1) != 0;
}

char const *AtaDevice::model()
{
    if (!_identified) {
        identify();
    }

    return _model;
}

char const *AtaDevice::serial()
{
    if (!_identified) {
        identify();
    }

    return _serial;
}

char const *AtaDevice::firmware()
{
    if (!_identified) {
        identify();
    }

    return _firmware;
}

AtaDevice::Type AtaDevice::type()
{
    softReset(); /* waits until master drive is ready again */
    outb(_ioPort + kAtaRegisterDriveSelect, 0xA0 | _slaveBit<<4);
    ioWait(); /* wait 400ns for drive select to work */
    uint8_t cl = inb(_ioPort + kAtaRegisterCylinderLow);	/* get the "signature bytes" */
    uint8_t ch = inb(_ioPort + kAtaRegisterCylinderHigh);

    /* differentiate ATA, ATAPI, SATA and SATAPI */
    if (cl==0x14 && ch==0xEB) return Type::PATAPI;
    if (cl==0x69 && ch==0x96) return Type::SATAPI;
    if (cl==0 && ch == 0) return Type::PATA;
    if (cl==0x3c && ch==0xc3) return Type::SATA;
    return Type::Unknown;
}

uint8_t AtaDevice::waitForStatus(int timeout) const
{
    uint8_t status;
    if (timeout > 0) {
        int i = 0;
        while ((status = inb(_ioPort + kAtaRegisterStatus)) & kAtaStatusBitBusy && (i < timeout)) i++;
    } else {
        while ((status = inb(_ioPort + kAtaRegisterStatus)) & kAtaStatusBitBusy);
    }
    return status;
}

uint8_t AtaDevice::waitUntilReady(bool errorCheck) const
{
    uint8_t status = 0;

    ioWait();

    status = waitForStatus(-1);

    if (errorCheck) {
        status = inb(_ioPort + kAtaRegisterStatus);
        if (status & kAtaStatusBitError) return 1;
        if (status & kAtaStatusBitDriveFault) return 1;
        if (!(status & kAtaStatusBitDRQ)) return 1;
    }

    return 0;
}

void AtaDevice::ioWait() const
{
    inb(_ioPort + 0x0C);
    inb(_ioPort + 0x0C);
    inb(_ioPort + 0x0C);
    inb(_ioPort + 0x0C);
}

void AtaDevice::softReset() const
{
    outb(_controlPort, 0x04);
    ioWait();
    outb(_controlPort, 0x00);
}

void AtaDevice::identify()
{
    // Set features register. 0 = PIO, 1 = DMA.
    outb(_ioPort + 1, 0);
    outb(_controlPort, 0);

    // select device
    outb(_ioPort + kAtaRegisterDriveSelect, 0xA0 | _slaveBit << 4);
    ioWait();

    switch (type()) {
        case Type::PATAPI:
        case Type::SATAPI:
            // send IDENTIFY PACKET DEVICE
            outb(_ioPort + 0x07, 0xA1); break;
        case Type::PATA:
        case Type::SATA:
            // send IDENTIFY DEVICE
            outb(_ioPort + 0x07, 0xEC); break;
        case Type::Unknown:
            return;
    }
    ioWait();

    // read in status
    waitUntilReady(false);

    // read in device information
    uint16_t identity[256];
    ata_identify_t *idstruct = (ata_identify_t *) identity;
    for (int i = 0; i < 256; ++i) {
        identity[i] = inw(_ioPort);
    }

    // hello endianness, my old friend...
    fixAtaWordString(idstruct->serial, 20);
    fixAtaWordString(idstruct->firmware, 8);
    fixAtaWordString(idstruct->model, 40);

    // transfer info to permanent, sane home
    strncpy(_serial, idstruct->serial, 20);
    strncpy(_firmware, idstruct->firmware, 8);
    strncpy(_model, idstruct->model, 40);

    printf("Sectors (48): %d\n", (int)idstruct->sectors_48);
    printf("Sectors (28): %d\n", (int)idstruct->sectors_28);


    _identified = true;
}

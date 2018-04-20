#include <arch/i386/device/storage/AtaDevice.hpp>
#include <sys/asm.h>

bool AtaDevice::isAttached() const
{
    softReset(); /* waits until master drive is ready again */
    outb(ioPort() + kAtaRegisterDriveSelect, 0xA0 | slaveBit() << 4);
    outb(ioPort() + kAtaRegisterLbaLow, 0x0);
    outb(ioPort() + kAtaRegisterLbaMid, 0x0);
    outb(ioPort() + kAtaRegisterLbaHigh, 0x0);
    outb(ioPort() + kAtaRegisterCommand, 0xEC);
    return waitForStatus(-1) != 0;
}

char const *AtaDevice::model()
{
    return _model;
}

char const *AtaDevice::serial()
{
    return _serial;
}

char const *AtaDevice::firmware()
{
    return _firmware;
}

AtaDevice::Type AtaDevice::type()
{
    softReset(); /* waits until master drive is ready again */
    outb(ioPort() + kAtaRegisterDriveSelect, 0xA0 | slaveBit()<<4);
    ioWait(); /* wait 400ns for drive select to work */
    uint8_t cl = inb(ioPort() + kAtaRegisterCylinderLow);	/* get the "signature bytes" */
    uint8_t ch = inb(ioPort() + kAtaRegisterCylinderHigh);

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
        while ((status = inb(ioPort() + kAtaRegisterStatus)) & kAtaStatusBitBusy && (i < timeout)) i++;
    } else {
        while ((status = inb(ioPort() + kAtaRegisterStatus)) & kAtaStatusBitBusy);
    }
    return status;
}

uint8_t AtaDevice::waitUntilReady(bool errorCheck) const
{
    uint8_t status = 0;

    ioWait();

    status = waitForStatus(-1);

    if (errorCheck) {
        status = inb(ioPort() + kAtaRegisterStatus);
        if (status & kAtaStatusBitError) return 1;
        if (status & kAtaStatusBitDriveFault) return 1;
        if (!(status & kAtaStatusBitDRQ)) return 1;
    }

    return 0;
}

void AtaDevice::ioWait() const
{
    inb(ioPort() + 0x0C);
    inb(ioPort() + 0x0C);
    inb(ioPort() + 0x0C);
    inb(ioPort() + 0x0C);
}

void AtaDevice::softReset() const
{
    outb(controlPort(), 0x04);
    ioWait();
    outb(controlPort(), 0x00);
}

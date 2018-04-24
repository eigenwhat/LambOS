#pragma once

#include <device/Device.hpp>
#include <stdint.h>
#include <cstring>
#include "AtaDeviceDescriptor.hpp"
#include "AtapiCommand.hpp"

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
     * Gets the type of device in the given slot.
     * @param primary Whether it's primary or secondary.
     * @param master Whether it's master or slave.
     * @return The probed type.
     */
    static Type typeOf(bool primary, bool master);

    /**
     * Constructs a blank ATA device representation for the given slot.
     * @param primary Whether it's primary or secondary.
     * @param master Whether it's master or slave.
     */
    AtaDevice(bool primary, bool master) : _primary(primary)
            , _slaveBit(slaveBit(master)), _ioPort(ioPort(primary)), _controlPort(controlPort(primary)) {}

    /**
     * Returns whether the device is primary or secondary. Relevant only for P-ATA/P-ATAPI.
     * @return true if primary, false otherwise.
     */
    bool isPrimary() const { return _primary; }

    /**
     * Returns whether the device is a master or slave. Relevant only for P-ATA/P-ATAPI.
     * @return True if master, false if slave.
     */
    bool isMaster() const { return !_slaveBit; }

    /**
     * Returns whether or not there is a device attached in this slot.
     * @return True if a device is there, false otherwise.
     */
    virtual bool isAttached() const;

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

    /**
     * The size of the device's sectors.
     * @return An unsigned 32-bit integer with the sector size in bytes.
     */
    uint32_t sectorSize() { return _descriptor.sectorSize(); }

    /**
     * The type of device. [P-ATA, P-ATAPI, S-ATA, S-ATAPI, unknown]
     * @return The Type enum value for this device.
     */
    Type type();

    /**
     * Reads in a sector.
     * @param address The linear block address of the sector to read.
     * @param buf The buffer to store the fetched data in. It should be at least
     *            the size of the device sector.
     * @param sectors The number of sectors to read. Defaults to 1.
     * @return true if successful, false otherwise.
     */
    bool read(uint64_t address, uint16_t *buf, size_t sectors = 1);

  private:
    // ====================================================
    // ATA base ports and bits
    // ====================================================
    static uint8_t slaveBit(bool master) { return static_cast<uint8_t>(master ? 0 : 1); }
    static uint16_t ioPort(bool primary) { return static_cast<uint16_t>(primary ? 0x1F0 : 0x170); }
    static uint16_t controlPort(bool primary) { return static_cast<uint16_t>(primary ? 0x3F6 : 0x376); }

    // ====================================================
    // ATA IO port offsets (port = ioPort() + kFoo)
    // ====================================================
    static constexpr uint8_t kAtaRegisterDataPort = 0x00;
    static constexpr uint8_t kAtaRegisterFeatureInfo = 0x01;
    static constexpr uint8_t kAtaRegisterSectorCount = 0x02;
    static constexpr uint8_t kAtaRegisterLbaLow = 0x03;
    static constexpr uint8_t kAtaRegisterSectorNo = 0x03;
    static constexpr uint8_t kAtaRegisterLbaMid = 0x04;
    static constexpr uint8_t kAtaRegisterCylinderLow = 0x04;
    static constexpr uint8_t kAtaRegisterLbaHigh = 0x05;
    static constexpr uint8_t kAtaRegisterCylinderHigh = 0x05;
    static constexpr uint8_t kAtaRegisterDriveSelect = 0x06;
    static constexpr uint8_t kAtaRegisterStatus = 0x07;
    static constexpr uint8_t kAtaRegisterCommand = 0x07;

    // ====================================================
    // Status byte bits
    // ====================================================

    /**
     * Indicates the drive is preparing to send/receive data (wait for it to clear).
     * In case of 'hang' (it never clears), do a software reset.
     */
    static constexpr uint8_t kAtaStatusBitBusy          = 0x80; // bit 7

    /** Bit is clear when drive is spun down, or after an error. Set otherwise. */
    static constexpr uint8_t kAtaStatusBitReady         = 0x40; // bit 6

    /** Drive Fault Error (does not set ERR). */
    static constexpr uint8_t kAtaStatusBitDriveFault    = 0x20; // bit 5

    /** Overlapped Mode Service Request. */
    static constexpr uint8_t kAtaStatusBitSRV           = 0x10; // bit 4

    /** Set when the drive has PIO data to transfer, or is ready to accept PIO data. */
    static constexpr uint8_t kAtaStatusBitDRQ           = 0x08; // bit 3

    /** Indicates an error occurred. Send a new command to clear it (or nuke it with a Software Reset). */
    static constexpr uint8_t kAtaStatusBitError         = 0x01; // bit 0

    /**
     * Waits for the device to not be busy, then returns the status.
     * @param timeout The number of attempts to make (1 attempt ~ 100ns). Set to
     *                -1 for no timeout.
     * @return The device status.
     */
    uint8_t waitForStatus(int timeout) const;

    /** Performs the IO operations to soft reset the ATA device. */
    void softReset() const;

    /**
     * Waits for the device to be ready.
     * @param errorCheck Additionally check that the device is not in an error state.
     * @return 0 if the device is in a good state, 1 if in an error state.
     */
    uint8_t waitUntilReady(bool errorCheck) const;

    /** Waits roughly 400ns by polling 4 times. */
    void ioWait() const;

    /** Identifies the device's name, model, firmware. */
    void identify();

    /**
     * Sends an ATA PACKET command plus the corresponding ATAPI command.
     * @param cmd The AtapiCommand to send
     * @param buf The buffer to read WORDS (uint16_t) into.
     * @param bufSize The size of the PIO buffer, in BYTES.
     * @return true if success, false if error
     */
    bool performPioAtapiOperation(const AtapiCommand &cmd, uint16_t *buf, size_t bufSize);

    /**
     * Reads in data via PIO from the device.
     * @param buf The buffer to read WORDS into.
     * @param bytesToRead The amount of data to read, in BYTES. Should be
     *                    divisible by 2, as we read in 2-byte words.
     */
    void pioRead(uint16_t *buf, uint32_t bytesToRead);

    /**
     * Writes out data via PIO to the device.
     * @param buf The buffer to read WORDS from.
     * @param bytesToRead The amount of data to write, in BYTES. Should be
     *                    divisible by 2, as we write out in 2-byte words.
     */
    void pioWrite(uint16_t *buf, uint32_t bytesToRead);

    bool const _primary;
    uint16_t const _slaveBit;
    uint16_t const _ioPort;
    uint16_t const _controlPort;
    bool _identified = false;
    AtaDeviceDescriptor _descriptor;
};

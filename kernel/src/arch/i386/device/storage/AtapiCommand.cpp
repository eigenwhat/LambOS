#include <arch/i386/device/storage/AtapiCommand.hpp>

namespace {

enum class ScsiCommandCode : uint8_t
{
    kTestUnitReady = 0x00,
    kRequestSense = 0x03,
    kFormatUnit = 0x04,
    kInquiry = 0x12,
    kStartStopUnit = 0x1B,
    kPreventAllowMediumRemoval = 0x1E,
    kReadFormatCapacities = 0x23,
    kReadCapacity = 0x25,
    kRead10 = 0x28,
    kWrite10 = 0x2A,
    kSeek10 = 0x2B,
    kWriteAndVerify10 = 0x2E,
    kVerify10 = 0x2F,
    kSynchronizeCache = 0x35,
    kWriteBuffer = 0x3B,
    kReadBuffer = 0x3C,
    kReadTocPmaAtip = 0x43,
    kGetConfiguration = 0x46,
    kGetEventStatusNotification = 0x4A,
    kReadDiscInformation = 0x51,
    kReadTrackInformation = 0x52,
    kReserveTrack = 0x53,
    kSendOpcInformation = 0x54,
    kModeSelect10 = 0x55,
    kRepairTrack = 0x58,
    kModeSense10 = 0x5A,
    kCloseTrackSession = 0x5B,
    kReadBufferCapacity = 0x5C,
    kSendCueSheet = 0x5D,
    kReportLuns = 0xA0,
    kBlank = 0xA1,
    kSecurityProtocolIn = 0xA2,
    kSendKey = 0xA3,
    kReportKey = 0xA4,
    kLoadUnloadMedium = 0xA6,
    kSetReadAhead = 0xA7,
    kRead12 = 0xA8,
    kWrite12 =0xAA,
    kReadMediaSerialNumber = 0xAB, // + 0x01
    kServiceActionIn12 = 0xAB, // + 0x01
    kGetPerformance = 0xAC,
    kReadDiscStructure = 0xAD,
    kSecurityProtocolOut = 0xB5,
    kSetStreaming = 0xB6,
    kReadCdMsf = 0xB9,
    kSetCdSpeed = 0xBB,
    kMechanismStatus = 0xBD,
    kReadCd = 0xBE,
    kSendDiscStructure = 0xBF,
};

}

AtapiCommand AtapiCommand::readCapacityCommand()
{
    AtapiCommand cmd;

    // set command
    cmd._packet.bytes[0] = (uint8_t)ScsiCommandCode::kReadCapacity;

    // blank rest
    std::memset(cmd._packet.bytes+1, 0, 11);

    return cmd;
}

AtapiCommand AtapiCommand::read12Command(uint32_t lba, uint32_t length)
{
    AtapiCommand cmd;

    // set command
    cmd._packet.bytes[0] = (uint8_t)ScsiCommandCode::kRead12;

    // blank reserved byte
    cmd._packet.bytes[1] = 0;

    // insert LBA into command packet in big-endian.
    cmd._packet.bytes[2] = static_cast<uint8_t>((lba >> 0x18) & 0xFF);
    cmd._packet.bytes[3] = static_cast<uint8_t>((lba >> 0x10) & 0xFF);
    cmd._packet.bytes[4] = static_cast<uint8_t>((lba >> 0x8) & 0xFF);
    cmd._packet.bytes[5] = static_cast<uint8_t>(lba & 0xFF);

    // set transfer length
    cmd._packet.bytes[6] = static_cast<uint8_t>((length >> 0x18) & 0xFF);
    cmd._packet.bytes[7] = static_cast<uint8_t>((length >> 0x10) & 0xFF);
    cmd._packet.bytes[8] = static_cast<uint8_t>((length >> 0x8) & 0xFF);
    cmd._packet.bytes[9] = static_cast<uint8_t>(length & 0xFF);

    // blank rest
    cmd._packet.bytes[10] = 0;
    cmd._packet.bytes[11] = 0;

    return cmd;
}

#include <device/storage/AtaDeviceDescriptor.hpp>

#include <cstring>

namespace {

inline uint32_t endianSwap(uint32_t l) {
    return ((((l) & 0xFF) << 24) | (((l) & 0xFF00) << 8) | (((l) & 0xFF0000) >> 8) | (((l) & 0xFF000000) >> 24));
}

void fixAtaWordString(char *str, size_t len)
{
    // ATAPI info is read one 16-bit word at a time, which means strings got scrambled by little-endian
    // (e.g. str is 214365 instead of 123456)
    for (size_t i = 0; i < len; i += 2) {
        char tmp = str[i + 1];
        str[i + 1] = str[i];
        str[i] = tmp;
    }
}

/** ATA/ATAPI IDENTIFY sends us device information in a 512-byte structure. */
struct AtaInfo
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
} __attribute__((packed));

}

void AtaDeviceDescriptor::readIdentity(uint16_t *rawInfo)
{
    if (rawInfo == nullptr) {
        return;
    }

    AtaInfo &ataInfo = *reinterpret_cast<AtaInfo*>(rawInfo);

    // Strings from IDENTIFY are not null terminated, but instead space padded to fill their full buffer size.
    // So, we store a buffer+1 size string and set the last byte to null. Voila!
    _serial[20] = 0;
    _firmware[8] = 0;
    _model[40] = 0;

    // hello endianness, my old friend...
    if (_littleEndian) {
        fixAtaWordString(ataInfo.serial, 20);
        fixAtaWordString(ataInfo.firmware, 8);
        fixAtaWordString(ataInfo.model, 40);
    }

    // transfer info to permanent, sane home
    strncpy(_serial, ataInfo.serial, 20);
    strncpy(_firmware, ataInfo.firmware, 8);
    strncpy(_model, ataInfo.model, 40);
    _lba28bitSectorCount = ataInfo.sectors_28;
    _lba48bitSectorCount = ataInfo.sectors_48;
    if (_lba48bitSectorCount || _lba48bitSectorCount) {
        _sectorSize = 512; // ATA devices have 512B sectors
    }
}

void AtaDeviceDescriptor::readAtapiCapacity(uint16_t *rawInfo)
{
    if (rawInfo == nullptr) {
        return;
    }

    uint32_t lba, blocks;
    memcpy(&lba, &rawInfo[0], sizeof(uint32_t));
    _atapiEndLba = _littleEndian ? endianSwap(lba) : lba;
    memcpy(&blocks, &rawInfo[2], sizeof(uint32_t));
    _sectorSize = _littleEndian ? endianSwap(blocks) : blocks;
}

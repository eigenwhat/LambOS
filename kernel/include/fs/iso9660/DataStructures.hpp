#pragma once

#include <stddef.h>
#include <stdint.h>

namespace iso9660 {

template <typename T> struct DualEndian
{
    T lsb;
    T msb;
}__attribute__((packed));

struct DateTime
{
    char year[4];
    char month[2];
    char day[2];
    char hour[2];
    char minute[2];
    char second[2];
    char hundredths[2];
    int8_t timezone;
} __attribute__((packed));

struct DirectoryInfo
{
    struct Date {
        uint8_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
        int8_t timezone;
    } __attribute__((packed));

    uint8_t length;
    uint8_t extendedAttributeLength;
    DualEndian<uint32_t> extentLba;
    DualEndian<uint32_t> extentLength;
    Date record_date;
    uint8_t flags;
    uint8_t interleaveUnits;
    uint8_t interleaveGap;
    DualEndian<uint16_t> sequenceNumber;
    uint8_t nameLength;
    char name[];
} __attribute__((packed));

struct PrimaryVolumeDescriptorInfo
{
    uint8_t type; // 0x01
    char id[5]; // CD001

    uint8_t version; // 0x01
    uint8_t _unused0; // 0x0

    char systemId[32];
    char volumeId[32];

    uint8_t _unused1[8]; // 0x0...

    DualEndian<uint32_t> spaceSize;

    uint8_t _unused2[32]; // 0x0...

    DualEndian<uint16_t> setSize;
    DualEndian<uint16_t> sequenceNumber;
    DualEndian<uint16_t> logicalBlockSize;
    DualEndian<uint32_t> pathTableSize;

    uint32_t pathTableLba_lsb;
    uint32_t optionalPathTableLba_lsb;
    uint32_t pathTableLba_msb;
    uint32_t optionalPathTableLba_msb;

    uint8_t root[34]; // DirectoryInfo

    char volumeSetId[128];
    char publisherId[128];
    char dataPreparerId[128];
    char applicationId[128];
    char copyrightFileId[38];
    char abstractFileId[36];
    char bibliographicFileId[37];

    DateTime creation;
    DateTime modification;
    DateTime expiration;
    DateTime effective;

    uint8_t fileStructureVersion; // 0x01
    uint8_t _unused_3; // 0x0

    uint8_t applicationUsed[512];
    uint8_t reserved[];
} __attribute__((packed));

} // namespace iso9660

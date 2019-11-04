#pragma once

#include <cstddef>
#include <stdint.h>

namespace iso9660 {

template <typename T> struct [[gnu::packed]] DualEndian
{
    T lsb;
    T msb;
};

struct [[gnu::packed]] DateTime
{
    char year[4];
    char month[2];
    char day[2];
    char hour[2];
    char minute[2];
    char second[2];
    char hundredths[2];
    int8_t timezone;
};

struct [[gnu::packed]] DirectoryInfo
{
    struct [[gnu::packed]] Date {
        uint8_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
        int8_t timezone;
    };

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

    // address for start of SUSP attributes.
    char *suspStart()
    {
        return name + nameLength + (nameLength % 2 ? 0 : 1);
    }

    // length of SUSP attribute set.
    std::ptrdiff_t suspLength()
    {
        std::ptrdiff_t lengthSoFar = suspStart() - (char*)this;
        return length - lengthSoFar;
    }
};

struct [[gnu::packed]] PrimaryVolumeDescriptorInfo
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
};

namespace rockridge {

/** Common header for Rock Ridge Interchange Protocol entries. */
struct [[gnu::packed]] EntryHeader
{
    char signature[2];
    uint8_t length;
    uint8_t entryVersion;
};

/** Rock Ridge "PX" entry. */
struct [[gnu::packed]] PosixFileEntryInfo
{
    EntryHeader header;
    char mode[8];
    char links[8];
    char uid[8];
    char gid[8];
    char serial[8];
};

/** Rock Ridge "PD" entry. */
struct [[gnu::packed]] PosixDeviceEntryInfo
{
    EntryHeader header;
    DualEndian<uint32_t> devtHigh;
    DualEndian<uint32_t> devtLow;
};

/** Rock Ridge "SL" entry. */
struct [[gnu::packed]] SymbolicLinkEntryInfo
{
    struct [[gnu::packed]] Component
    {
        uint8_t flags;
        uint8_t length;
        char name[];
    };

    EntryHeader header;
    uint8_t flags;
    Component components[];
};

/** Rock Ridge "NM" entry. */
struct [[gnu::packed]] AltNameEntryInfo
{
    EntryHeader header;
    uint8_t flags;
    char name[];
};

/** Rock Ridge "CL" entry. */
struct [[gnu::packed]] ChildLinkEntryInfo
{
    EntryHeader header;
    DualEndian<uint32_t> childLba;
};

/** Rock Ridge "PL" entry. */
struct [[gnu::packed]] ParentLinkEntryInfo
{
    EntryHeader header;
    DualEndian<uint32_t> parentLba;
};

/** Rock Ridge "RE" entry. */
struct [[gnu::packed]] RelocatedDirectoryEntryInfo
{
    EntryHeader header;
};

/** Rock Ridge "TF" entry. */
struct [[gnu::packed]] TimeStampEntryInfo
{
    EntryHeader header;
    uint8_t flags;
    uint8_t timestamps[]; // depending on flag bit 7, either 7 or 17 bit stamps.
};

/** Rock Ridge "SF" entry. */
struct [[gnu::packed]] SparseFileEntryInfo
{
    EntryHeader header;
    DualEndian<uint32_t> virtualSizeHigh;
    DualEndian<uint32_t> virtualSizeLow;
    uint8_t tableDepth;
};

} // namespace rockridge

} // namespace iso9660

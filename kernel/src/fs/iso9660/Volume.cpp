#include <fs/iso9660/Volume.hpp>

#include <fs/iso9660/DataStructures.hpp>
#include <util/StaticList.hpp>
#include <cstdio>
#include <fs/iso9660/DirectoryEntry.hpp>
#include <Kernel.hpp>

namespace iso9660 {

namespace {

enum class VolumeDescriptorType : int
{
    kBootRecord = 0,
    kPrimaryVolume = 1,
    kSupplementaryVolume = 2,
    kUnknown,
    kSetTerminator = 255
};

VolumeDescriptorType getDescriptorType(uint8_t *buf)
{
    uint8_t type = buf[0];
    switch (type) {
        case 0:
            return VolumeDescriptorType::kBootRecord;
        case 1:
            return VolumeDescriptorType::kPrimaryVolume;
        case 2:
            return VolumeDescriptorType::kSupplementaryVolume;
        case 255:
            return VolumeDescriptorType::kSetTerminator;
        default:
            return VolumeDescriptorType::kUnknown;
    }
}

}

Volume::Volume(AtaDevice &device) : ::Volume(device) {}

void Volume::init()
{
    uint64_t lba = 0x10;
    bool reachedEnd = false;
    sys::StaticList<uint8_t> buf{_parentDevice->sectorSize()};
    do {
        _parentDevice->read(lba, (uint16_t *)buf.get());
        auto type = getDescriptorType(buf);
        if (type == VolumeDescriptorType::kPrimaryVolume) {
            break;
        } else if (type == VolumeDescriptorType::kSetTerminator) {
            reachedEnd = true;
            break;
        }
        ++lba;
    } while (!reachedEnd);

    if (reachedEnd) {
        // error?
        kernel->panic("Failed to find ISO 9660 Primary Volume Descriptor.");
        return;
    }

    // okay, we have the volume descriptor
    auto descriptor = reinterpret_cast<PrimaryVolumeDescriptorInfo *>(buf.get());

    // set root directory entry
    auto *rootEntry = reinterpret_cast<DirectoryInfo *>(&(descriptor->root));
    _root = make_arc<iso9660::DirectoryEntry>(*rootEntry, *this);

    // get volume label
    size_t volumeIdLength = 0;
    for (char c : descriptor->volumeId)
    {
        if (c == ' ') break;
        ++volumeIdLength;
    }
    _label.append(descriptor->volumeId, volumeIdLength);
}

::DirectoryEntry *Volume::root() const
{
    return _root.get();
}

::DirectoryEntry *Volume::find(char const *path) const
{
    return _root->find(path);
}

}

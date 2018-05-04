#include <fs/iso9660/Iso9660.hpp>
#include <fs/iso9660/Volume.hpp>
#include <util/Array.hpp>
#include <cstring>

FileSystem &Iso9660::instance()
{
    static Iso9660 instance{};
    return instance;
}

bool Iso9660::hasFileSystem(AtaDevice &device)
{
    // Bytes 1-6 of an ISO9660 volume descriptor contain the string "CD001",
    // so we check for it. The first volume descriptor is at 0x10.
    const auto volumeDescriptor = 0x10;
    Array<uint8_t> buf{device.sectorSize()};
    device.read(volumeDescriptor, (uint16_t *)buf.get());
    Array<char> cd001{6};
    memset(cd001.get(), 0, 6);
    strncpy(cd001, (char*)&buf[1], 5);

    return !strcmp(cd001.get(), "CD001");
}

Volume *Iso9660::createVolume(AtaDevice &device)
{
    if (hasFileSystem(device)) {
        auto vol = new iso9660::Volume(device);
        vol->init();
        return vol;
    }

    return nullptr;
}

#include <fs/iso9660/Iso9660.hpp>
#include <fs/iso9660/Volume.hpp>
#include <util/StaticList.hpp>
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
    sys::StaticList<uint8_t> buf{device.sectorSize()};
    device.read(volumeDescriptor, (uint16_t *)buf.get());
    char *cd001 = reinterpret_cast<char*>(&buf[1]);
    cd001[5] = 0;
    return !strcmp(cd001, "CD001");
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

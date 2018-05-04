#pragma once

#include <device/storage/AtaDevice.hpp>
#include <fs/Volume.hpp>
#include <fs/iso9660/DirectoryEntry.hpp>

namespace iso9660 {

class Volume : public ::Volume
{
  public:
    Volume(AtaDevice &device);

    void init();

    int unmount() override;

    ::DirectoryEntry *root() override;

  private:
    ArcPtr<DirectoryEntry> _root;
};

}

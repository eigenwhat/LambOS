#pragma once

#include <device/storage/AtaDevice.hpp>
#include <fs/DirectoryEntry.hpp>
#include <Object.hpp>

class Volume : public virtual Object
{
  public:
    Volume() : _parentDevice(nullptr) {}
    Volume(AtaDevice &device) : _parentDevice(&device) {}

    char const *label() { return _label.cstr(); }

    /**
     * Unmounts the volume.
     * @return 0 if success, an error code otherwise.
     */
    virtual int unmount() = 0;

    /**
     * Returns the DirectoryEntry corresponding to the root of the Volume.
     * @return A pointer to the DirectoryEntry.
     */
    virtual DirectoryEntry *root() = 0;

    AtaDevice *parentDevice() const { return _parentDevice.get(); }

  protected:
    String _label;
    ArcPtr<AtaDevice> _parentDevice;
};

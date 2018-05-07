#pragma once

#include <device/storage/AtaDevice.hpp>
#include <fs/DirectoryEntry.hpp>
#include <Object.hpp>

class Volume : public virtual Object
{
  public:
    Volume() : _parentDevice(nullptr) {}
    Volume(AtaDevice &device) : _parentDevice(&device) {}

    /**
     * The label of the volume.
     * @return The String containing the volume's label.
     */
    String const &label() { return _label; }

    /**
     * Returns the DirectoryEntry corresponding to the root of the Volume.
     * @return A pointer to the DirectoryEntry.
     */
    virtual DirectoryEntry *root() const = 0;

    /**
     * Retrieves the directory entry for the given absolute path if it exists.
     * @param path The path to search out.
     * @return The DirectoryEntry corresponding to that path, or `nullptr` if no
     *         such thing exists.
     */
    virtual DirectoryEntry *find(char const *path) const = 0;

    /**
     * The physical device the Volume is on.
     * @return The AtaDevice instance.
     */
    AtaDevice *parentDevice() const { return _parentDevice.get(); }

  protected:
    String _label;
    ArcPtr<AtaDevice> _parentDevice;
};

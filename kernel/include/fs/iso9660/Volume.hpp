#pragma once

#include <device/storage/AtaDevice.hpp>
#include <fs/Volume.hpp>
#include <fs/iso9660/DirectoryEntry.hpp>

namespace iso9660 {

class Volume : public ::Volume
{
  public:
    /**
     * Constructs a Volume. Most of the heavy-lifting is deferred to the init()
     * method.
     * @param device The device containing the Volume.
     */
    Volume(AtaDevice &device);

    /**
     * Reads from the device, initializing the in-memory representation of this
     * Volume.
     */
    void init();

    /**
     * Returns the DirectoryEntry corresponding to the root of the Volume.
     * @return A pointer to the DirectoryEntry.
     */
    ::DirectoryEntry *root() const override;

    /**
     * Retrieves the directory entry for the given absolute path if it exists.
     * @param path The path to search out.
     * @return The DirectoryEntry corresponding to that path, or `nullptr` if no
     *         such thing exists.
     */
    ::DirectoryEntry *find(char const *path) const override;

  private:
    sys::ArcPtr<DirectoryEntry> _root;
};

}

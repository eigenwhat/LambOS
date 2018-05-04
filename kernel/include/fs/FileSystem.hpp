#pragma once

#include <device/storage/AtaDevice.hpp>
#include <fs/Volume.hpp>

class FileSystem
{
  public:
    char const *name() { return _name; }

    /**
     * Reports if the device (or partition) has this file system.
     * @param device The device to check.
     * @return `true` if it is of this file system, `false` otherwise.
     */
    virtual bool hasFileSystem(AtaDevice &device) = 0;

    /**
     * Constructs a Volume of this file system from the device (or partition).
     * @param device The device to build a Volume object around.
     * @return The volume object, or nullptr if the device is invalid.
     */
    virtual Volume *createVolume(AtaDevice &device) = 0;

  protected:
    FileSystem(char const *name) : _name(name) {}

  private:
    char const *_name = nullptr;
};
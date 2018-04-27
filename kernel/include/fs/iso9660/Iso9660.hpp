#pragma once

#include <fs/FileSystem.hpp>

class Iso9660 : public FileSystem
{
  public:
    /** Returns singleton instance. */
    static FileSystem &instance();

    bool hasFileSystem(AtaDevice &device) override;

  private:
    Iso9660() : FileSystem("ISO9660") {}
};
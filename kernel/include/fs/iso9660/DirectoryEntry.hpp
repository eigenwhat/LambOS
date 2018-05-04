#pragma once

#include <fs/DirectoryEntry.hpp>
#include <fs/iso9660/DataStructures.hpp>
#include <util/String.hpp>

namespace iso9660 {

class Volume;

class DirectoryEntry : public ::DirectoryEntry
{
  public:
    DirectoryEntry(DirectoryInfo &info, Volume &volume);


    List<String> *readdir() override;

    /**
     * Does nothing. Read-only file system.
     * @return -1.
     */
    int mkfile(char const *) override;

    /**
     * Does nothing. Read-only file system.
     * @return -1.
     */
    int mkdir(char const *) override;

    /**
     * Does nothing. Read-only file system.
     * @return -1.
     */
    int rmdir(char const *) override;

    /**
     * Does nothing. Read-only file system.
     * @return -1.
     */
    int link(char const *, char const *) override;

    /**
     * Does nothing. Read-only file system.
     * @return -1.
     */
    int unlink(char const *) override;

  private:
    uint32_t _extentLba;
    uint32_t _extentLength;
};

}

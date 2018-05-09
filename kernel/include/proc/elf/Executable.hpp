#pragma once

#include <fs/DirectoryEntry.hpp>
#include <Object.hpp>

namespace elf {

class Executable : public Object
{
  public:
    /**
     * Checks if the DirectoryEntry points to a valid ELF binary.
     * This will open the entry's file and read from it, looking for the ELF
     * header.
     * @param entry The entry to check.
     * @return `true` if it's a valid ELF binary, `false` otherwise.
     */
    static bool isElf(DirectoryEntry const &entry);

    Executable(DirectoryEntry &entry);

  private:
    ArcPtr<InputStream> _fileStream;
};

}
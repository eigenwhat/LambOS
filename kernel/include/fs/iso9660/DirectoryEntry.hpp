#pragma once

#include <fs/DirectoryEntry.hpp>
#include <fs/iso9660/DataStructures.hpp>
#include <util/StringTokenizer.hpp>

namespace iso9660 {

class Volume;

class DirectoryEntry : public ::DirectoryEntry
{
  public:
    DirectoryEntry(DirectoryInfo &info, Volume &volume);

    /**
     * Retrieves the directory entry for the given relative path if it exists.
     * @param path The path to search out.
     * @return The DirectoryEntry corresponding to that path, or `nullptr` if no
     *         such thing exists.
     */
    ::DirectoryEntry *find(char const *path) override;

    /**
     * Reads the names of the contents of the directory.
     * If the DirectoryEntry describes a file, `nullptr` is returned.
     * @return A list of the directory contents, or `nullptr` if this is a file.
     */
    sys::Maybe<sys::LinkedList<sys::String>> readdir() const override;

    /**
     * Prepares an InputStream for reading the contents of the file.
     * @return The InputStream, or `nullptr` if this is a directory.
     */
    sys::UniquePtr<sys::InputStream> fileStream() const override;

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
    ::DirectoryEntry *find(sys::StringTokenizer &tokenizer);

    uint32_t _extentLba = 0;
    uint32_t _extentLength = 0;
};

}

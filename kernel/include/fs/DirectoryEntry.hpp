#pragma once

#include <Memory.hpp>
#include <cstdint>
#include <util/Maybe.hpp>
#include <util/LinkedList.hpp>
#include <util/String.hpp>
#include <io/InputStream.hpp>

class Volume;

// not sure if this or "Dirent" is more identifiable
class DirectoryEntry
{
  public:
    enum class Type {
        Unknown,
        File,
        Directory
    };

    DirectoryEntry(Volume &volume) : _volume(volume) {}

    DirectoryEntry(Volume &volume, Type type, sys::String const &name)
            : _volume(volume), _type(type), _name(name) {}

    virtual ~DirectoryEntry() = default;

    /**
     * Returns the volume this directory entry belongs to.
     * @return The Volume object.
     */
    Volume &volume() const { return _volume; }

    /**
     * The type of filesystem object the entry describes.
     * @return The Type.
     */
    Type type() const { return _type; }

    /**
     * The name of the file or directory.
     * @return A C string containing the name of the file or directory.
     */
    char const *name() const { return _name.cstr(); }

    /** Returns `true` if the DirectoryEntry points to a file. */
    bool isFile() const { return _type == Type::File; }

    /** Returns `true` if the DirectoryEntry points to a directory. */
    bool isDir() const { return _type == Type::Directory; }

    /**
     * Retrieves the directory entry for the given relative path if it exists.
     * @param path The path to search out.
     * @return The DirectoryEntry corresponding to that path, or `nullptr` if no
     *         such thing exists.
     */
    virtual DirectoryEntry *find(char const *path) = 0;

    /**
     * Reads the names of the contents of the directory.
     * If the DirectoryEntry describes a file, `nullptr` is returned.
     * @return A list of the directory contents, or `nullptr` if this is a file.
     */
    virtual sys::Maybe<sys::LinkedList<sys::String>> readdir() const = 0;

    /**
     * Prepares an InputStream for reading the contents of the file.
     * @return The InputStream, or `nullptr` if this is a directory.
     */
    virtual sys::UniquePtr<sys::InputStream> fileStream() const = 0;

    /**
     * Creates a new file in the directory of this DirectoryEntry.
     * @param name The name of the file.
     * @return uh...
     */
    virtual int mkfile(char const *name) = 0;
    virtual int mkdir(char const *name) = 0;
    virtual int rmdir(char const *name) = 0;
    virtual int link(char const *oldpath, char const *newpath) = 0;
    virtual int unlink(char const *name) = 0;

  protected:
    void setType(Type type) { _type = type; }
    void setName(sys::String const &name) { _name = name; }

  private:
    Volume &_volume;
    Type _type = Type::Unknown;
    sys::String _name;
};

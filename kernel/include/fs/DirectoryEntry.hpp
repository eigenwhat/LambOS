#pragma once

#include <Object.hpp>
#include <stdint.h>
#include <util/List.hpp>
#include <util/String.hpp>
#include <io/InputStream.hpp>

class Volume;

// not sure if this or "Dirent" is more identifiable
class DirectoryEntry : public Object
{
  public:
    enum class Type {
        Unknown,
        File,
        Directory
    };

    DirectoryEntry(Volume &volume) : _volume(volume) {}

    DirectoryEntry(Volume &volume, Type type, String const &name)
            : _volume(volume), _type(type), _name(name) {}

    /**
     * Returns the volume this directory entry belongs to.
     * @return The Volume object.
     */
    Volume &volume() { return _volume; }

    /**
     * The type of filesystem object the entry describes.
     * @return The Type.
     */
    Type type() { return _type; }

    /**
     * The name of the file or directory.
     * @return A C string containing the name of the file or directory.
     */
    char const *name() { return _name.cstr(); }

    /** Returns `true` if the DirectoryEntry points to a file. */
    bool isFile() { return _type == Type::File; }

    /** Returns `true` if the DirectoryEntry points to a directory. */
    bool isDir() { return _type == Type::Directory; }

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
    virtual List<String> *readdir() = 0;

    /**
     * Prepares an InputStream for reading the contents of the file.
     * @return The InputStream, or `nullptr` if this is a directory.
     */
    virtual InputStream *fileStream() = 0;

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
    void setName(String const &name) { _name = name; }

  private:
    Volume &_volume;
    Type _type = Type::Unknown;
    String _name;
};

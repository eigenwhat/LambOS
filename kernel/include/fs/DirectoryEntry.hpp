#pragma once

#include <Object.hpp>
#include <stdint.h>
#include <util/LinkedList.hpp>
#include <util/String.hpp>

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

    DirectoryEntry(Volume &volume, Type type, String const &name)
            : _volume(volume), _type(type), _name(name) {}

    /**
     * Returns the volume this directory entry belongs to.
     * @return The Volume object.
     */
    Volume &volume() { return _volume; }

    Type type() { return _type; }

    char const *name() { return _name.cstr(); }

    bool isFile() { return _type == Type::File; }
    bool isDir() { return _type == Type::Directory; }

//    virtual DirectoryEntry *namei(char const *path) = 0;
//    virtual struct fs_file *file_open(struct fs_dirent *d, uint8_t mode) = 0;
//    virtual int close(struct fs_dirent *d) = 0;

    /**
     * Reads the names of the contents of the directory.
     * @return A list of the directory contents.
     */
    virtual List<String> *readdir() = 0;
    virtual int mkfile(char const *name) = 0;
    virtual int mkdir(char const *name) = 0;
    virtual int rmdir(char const *name) = 0;
    virtual int link(char const *oldpath, char const *newpath) = 0;
    virtual int unlink(char const *name) = 0;

  protected:
    void setType(Type type) { _type = type; }

  private:
    Volume &_volume;
    Type _type = Type::Unknown;
    String _name;
};

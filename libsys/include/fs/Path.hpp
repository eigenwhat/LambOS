#pragma once

#include <util/ArrayList.hpp>
#include <util/String.hpp>

namespace sys {

/**
 * A Path represents a possible path on a filesystem. No checks are made as to
 * whether or not the path exists on a filesystem; it handles only the
 * syntactical aspects of a path.
 */
class Path
{
  public:
    /**
     * Creates a Path object with the given String.
     * @param str The String to process.
     */
    Path(String const &str) : Path(str.cstr()) {}

    /**
     * Creates a Path object with the given C string.
     * @param str The C string to process.
     */
    Path(char const *str);

    /**
     * Returns the Path as a String.
     * @return The stringified path.
     */
    String pathString() const;

    /**
     * Returns the last path component.
     * @return A String representation of the last path component.
     */
    String const &lastPathComponent() const;

    /**
     * Returns the file extension of the last path component, if the path is to
     * a file.
     * @return The extension. (e.g. "zip" if lastPathComponent() == "file.zip")
     */
    String extension() const;

    /**
     * Returns whether or not the path is absolute (e.g. starts with "/")
     * @return `true` if absolute, `false` otherwise.
     */
    bool isAbsolute() const { return _isAbsolute; }

    /**
     * Returns whether of not the path ends with a delimiter. (e.g. "/")
     * @return `true` if the path points to a directory, `false` otherwise.
     */
    bool isDirectory() const { return _isDirectory; }

  private:
    ArrayList<String> _pathComponents;
    bool _isAbsolute = false;
    bool _isDirectory = false;
};

} // libsys namespace

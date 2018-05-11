#pragma once

#include <Object.hpp>
#include <io/InputStream.hpp>
#include <util/DynamicArray.hpp>
#include <cstdio>

class FileReader : public Object
{
  public:
    static constexpr int kEOF = -1;

    /**
     * Constructs a FileReader with the provided InputStream.
     * @param fileStream The InputStream to read the file from. The file is
     * considered to start from the first byte read.
     */
    FileReader(InputStream *fileStream) : _fileStream(fileStream)
            , _eofReached(fileStream == nullptr)
    {}

    /**
     * Reads the byte at the given offset.
     * If that part of the file hasn't been read yet, then the stream will be
     * read up to that offset.
     * @param offset The offset of the byte to read.
     * @return The byte, or kEOF if the offset is past EOF.
     */
    int read(size_t offset);

    /**
     * Reads a number of bytes starting from a given offset.
     * @param buf A buffer to store the bytes.
     * @param offset The offset to start reading at.
     * @param bytesToRead The number of bytes to read.
     * @return The number of bytes actually read.
     */
    size_t read(uint8_t *buf, size_t offset, size_t bytesToRead);

    /**
     * Returns the file as read so far.
     * @return A pointer to the byte array.
     */
    uint8_t const *bytesRead() const { return _fileBytes; }

    /** Whether or not the end-of-file (EOF) has been reached. */
    bool eofReached() const { return _eofReached; }

    /** Reads the entire file into memory. */
    void readAll();

  private:
    void markEofReached()
    {
        _eofReached = true;
        _fileStream = nullptr;
    }

    ArcPtr<InputStream> _fileStream;
    DynamicArray<uint8_t> _fileBytes{512};
    size_t _fileExtent = 0;
    bool _eofReached = false;
};

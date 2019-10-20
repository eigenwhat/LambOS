#pragma once

#include <Object.hpp>
#include <io/InputStream.hpp>
#include <util/DynamicArray.hpp>
#include <cstdio>

class FileReader : public Object
{
  public:
    static inline const auto kEOF = InputStream::kEndOfStream;

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
    InputStream::Byte read(size_t offset);

    /**
     * Reads a number of bytes starting from a given offset.
     * @param buf A buffer to store the bytes.
     * @param offset The offset to start reading at.
     * @param bytesToRead The number of bytes to read.
     * @return The number of bytes actually read.
     */
    size_t read(std::byte *buf, size_t offset, size_t bytesToRead);

    /**
     * Returns the file as read so far.
     * @see bytesRead() returns the actual extent of the data in the array.
     * @return A pointer to the byte array.
     */
    [[nodiscard]] std::byte const *bytes() const { return _fileBytes; }

    /**
     * Returns the number of bytes read so far.
     * @return The number of bytes read.
     */
    [[nodiscard]] size_t bytesRead() const { return _fileExtent; }

    /** Whether or not the end-of-file (EOF) has been reached. */
    [[nodiscard]] bool eofReached() const { return _eofReached; }

    /** Reads the entire file into memory up front. */
    void readAll();

  private:
    void markEofReached()
    {
        _eofReached = true;
        _fileStream = nullptr;
    }

    ArcPtr<InputStream> _fileStream;
    DynamicArray<std::byte> _fileBytes{512};
    size_t _fileExtent = 0;
    bool _eofReached = false;
};

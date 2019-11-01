//
// Created by Martin Miralles-Cordal on 5/10/2018.
//

#include <io/FileReader.hpp>

namespace _ns_LIBSYS {

InputStream::Byte FileReader::read(size_t offset)
{
    if (offset >= _fileExtent) {
        if (_eofReached) {
            return kEOF;
        }

        // grow in-memory cache if needed
        if (offset >= _fileBytes.capacity()) {
            auto doubleCapacity = _fileBytes.capacity() * 2;
            auto newSize = offset < doubleCapacity ? doubleCapacity
                                                   : offset;
            _fileBytes.resize(newSize);
        }

        // fill file cache up to offset
        auto const expectedBytes = 1 + (offset - _fileExtent);
        auto bytesRead = _fileStream->read(&_fileBytes[_fileExtent], expectedBytes);
        _fileExtent += bytesRead;

        if (expectedBytes != bytesRead) {
            // hit EOF before reaching offset
            markEofReached();
            return kEOF;
        }
    }

    return _fileBytes[offset];
}

size_t FileReader::read(std::byte *buf, size_t offset, size_t bytesToRead)
{
    if (bytesToRead == 0) {
        return 0;
    }

    // first, read up to the end of the range if needed
    auto lastIndex = offset + bytesToRead - 1;
    read(offset + bytesToRead - 1);
    size_t bytesRead = 0;
    for (size_t i = offset; i <= lastIndex; ++i) {
        if (i >= _fileExtent) {
            // we've reached EOF
            break;
        } else {
            buf[i-offset] = _fileBytes[i];
            ++bytesRead;
        }
    }

    return bytesRead;
}

void FileReader::readAll()
{
    size_t i = 0;
    while (read(i++) != InputStream::kEndOfStream);
}

} // libsys namespace

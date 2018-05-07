#pragma once

#include <Object.hpp>

#include <stdint.h>
#include <stddef.h>

/** A stream of bytes that can be read from. */
class InputStream : public Object
{
public:
    static constexpr int kEndOfStream = -1;
    /**
     * Returns an estimate of the number of bytes available for reading without
     * blocking for more data.
     * @return The estimated number of bytes available to read/skip.
     */
    virtual size_t available() const { return 0; }

    /** Closes the stream, releasing any resources associated with it. */
    virtual void close() {}

    /**
     * Marks the current position in the stream.
     *
     * If a position is marked, future calls to reset() will return to this
     * position, acting as though they had not been read. Not all streams
     * support this; if the stream does, markSupported() will return true.
     *
     * @param readLimit The number of bytes read after which the mark becomes
     *                  invalid.
     */
    virtual void mark(size_t) {}

    /**
     * Returns whether or not marking a reset point for the stream is supported.
     * @return `true` if supported, `false` otherwise.
     */
    virtual bool markSupported() const { return false; }

    /**
     * Reads the next byte of data from the stream.
     * @return The next byte of data, or `kEndOfStream` if the end of the stream
     *         has been reached.
     */
    virtual int read() = 0;

    /**
     * Reads in the next `bytesToRead` bytes of data from the stream, storing
     * them in the provided byte buffer.
     * @param bytes The buffer in which to store the read data.
     * @param bytesToRead The maximum number of bytes to read.
     * @return The number of bytes actually read.
     */
    virtual int read(uint8_t *bytes, size_t bytesToRead)
    {
        int bytesRead = 0;
        for (; bytesToRead > 0; --bytesToRead) {
            int byte = read();
            if (byte == kEndOfStream) break; // reached end of stream.
            *bytes++ = byte;
            ++bytesRead;
        }

        return bytesRead;
    }

    /**
     * Rolls back the stream to the position last marked by the mark() method.
     *
     * If mark() was never called, the marked point has become invalid, or
     * markSupported() returns false, this method does nothing.
     */
    virtual void reset() {}
};

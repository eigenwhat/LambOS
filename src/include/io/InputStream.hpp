#include <stddef.h>
#include <stdint.h>

class InputStream {
public:
	virtual size_t available() { return 0; }
	virtual void close() { }
	virtual void mark(size_t readLimit) { }
	virtual bool markSupported() { return false; }
	virtual uint8_t read() = 0;
	virtual int32_t read(uint8_t *bytes, size_t bytesToRead) { for(; bytesToRead--; *bytes++ = read(); }
	virtual void reset() { }
};

#pragma once
#include <stddef.h>
#include <stdint.h>
#include <ITerminal.hpp>

extern "C" void set_gdt(void *gdt, size_t size);

#define GDT_SIZE 3

class Kernel
{
public:
	Kernel() : _stdout(NULL) {}
	void setStdout(ITerminal &terminal) { this->_stdout = &terminal; }
	ITerminal* stdout() { return this->_stdout; }
	void kerror(const char *errorMessage);
private:
	ITerminal *_stdout;
};

struct GDTEntry
{
public:
	GDTEntry() : base(0), limit(0), type(0) {}
	GDTEntry(uint32_t base, uint32_t limit, uint32_t type) : base(base), limit(limit), type(type) {}
	uint32_t base;
	uint32_t limit;
	uint8_t type;
};

class GlobalDescriptorTable
{
public:
	GlobalDescriptorTable(uint64_t *address, size_t entries) : gdt(address), size(entries) {}
	void encodeEntry(uint8_t entryNumber, GDTEntry source);
	void writeToMemory() { set_gdt(this->gdt, sizeof(uint64_t)*size); }
private:
	uint64_t *gdt;
	size_t size;
};

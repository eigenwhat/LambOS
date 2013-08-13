#pragma once
#include <stddef.h>
#include <stdint.h>

extern "C" void set_idt(void *idt, size_t size);

class IDTEntry
{
public:
	IDTEntry() : offset(0), selector(0), type_attr(0) {}
	IDTEntry(uint16_t selector, uint32_t offset, uint8_t type_attr) : offset(offset), selector(selector), type_attr(type_attr) {}
	friend class InterruptDescriptorTable;
private:
	uint32_t offset;
	uint16_t selector;
	uint8_t type_attr;
};

class InterruptDescriptorTable
{
public:
	InterruptDescriptorTable(uint64_t *address, size_t entries) : idt(address), size(entries) {}
	void encodeEntry(uint8_t entryNumber, IDTEntry source);
	void writeToMemory() { set_idt(this->idt, sizeof(uint64_t)*size); }
private:
	uint64_t *idt;
	size_t size;
};

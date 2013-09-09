#pragma once
#include <stddef.h>
#include <stdint.h>
#include "RegisterTable.h"

extern "C" void set_idt(void *idt, size_t size);

#define IDT_SIZE 256

struct InterruptServiceRoutine
{
	virtual void operator()(RegisterTable&) = 0;
};

class IDTEntry
{
public:
	IDTEntry() : offset(0), selector(0), type_attr(0) {}
	IDTEntry(uint32_t offset, uint8_t type_attr) : offset(offset), selector(0x08), type_attr(type_attr) {}
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
	void encodeEntry(uint8_t entryNumber, IDTEntry source);
	void encodeHWExceptionISRs();
	void install() { set_idt(this->idt, sizeof(uint64_t) * IDT_SIZE); }
	void callInterruptServiceRoutine(uint8_t interruptNumber, RegisterTable &registers) { (*isr[interruptNumber])(registers); }
private:
	InterruptServiceRoutine *isr[IDT_SIZE];
	uint64_t idt[IDT_SIZE];
};
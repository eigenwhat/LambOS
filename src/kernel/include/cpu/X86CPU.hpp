#pragma once
#include <stddef.h>
#include <stdint.h>
#include <cpu/GlobalDescriptorTable.hpp>
#include <cpu/InterruptDescriptorTable.hpp>
#include <cpu/CPU.hpp>

class X86CPU : public CPU {
public:
	X86CPU();
	virtual void install();
	InterruptDescriptorTable *idt() { return &_idt; }
private:
	GlobalDescriptorTable _gdt;
	InterruptDescriptorTable _idt;
	TaskStateSegment _tss;
};

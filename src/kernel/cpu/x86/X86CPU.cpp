#include <cpu/X86CPU.hpp>
#include <string.h>

X86CPU::X86CPU()
{
	// Set up GDT
    _gdt.encodeEntry(0, GDTEntry());
    _gdt.encodeEntry(1, GDTEntry(0, 0xffffffff, 0x9A));
    _gdt.encodeEntry(2, GDTEntry(0, 0xffffffff, 0x92));
    _gdt.encodeEntry(3, GDTEntry(0, 0xffffffff, 0xFA));
    _gdt.encodeEntry(4, GDTEntry(0, 0xffffffff, 0xF2));

    memset(&_tss, 0x0, sizeof(_tss));
    _tss.ss0 = 0x10;
    _tss.esp0 = 0x0;
    _tss.iomap_base = sizeof(_tss);

    _gdt.encodeEntry(5, GDTEntry((uint32_t)&_tss, sizeof(_tss), 0x89));

    // Set up IDT
    _idt.encodeHWExceptionISRs();
}

void X86CPU::install() 
{
	// install GDT
	_gdt.install();

	// install TSS
    _gdt.installTSS(5);

    // install IDT
    _idt.install();
}

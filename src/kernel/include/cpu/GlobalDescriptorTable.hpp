#pragma once
#include <stddef.h>
#include <stdint.h>

extern "C" void set_gdt(void *gdt, size_t size);

class GDTEntry
{
public:
	GDTEntry() : base(0), limit(0), type(0) {}
	GDTEntry(uint32_t base, uint32_t limit, uint32_t type) : base(base), limit(limit), type(type) {}
	friend class GlobalDescriptorTable;
private:
	uint32_t base;
	uint32_t limit;
	uint8_t type;
};

class GlobalDescriptorTable
{
public:
	GlobalDescriptorTable(uint64_t *address, size_t entries) : gdt(address), size(entries) {}
	void encodeEntry(uint8_t entryNumber, GDTEntry source);
	void install() { set_gdt(this->gdt, sizeof(uint64_t)*size); }
	void installTSS(uint8_t entryIndex);
private:
	uint64_t *gdt;
	size_t size;
};

typedef struct tss_entry {
    uint32_t prev_tss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__ ((packed)) TaskStateSegment;

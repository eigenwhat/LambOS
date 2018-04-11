#pragma once

#include <stddef.h>
#include <stdint.h>

#define GDT_SIZE 6

extern "C" void set_gdt(void *gdt, size_t size);

class GDTEntry
{
public:
    /**
     * Access byte ("type") structure (https://wiki.osdev.org/GDT)
     *
     * 8    7       5    4    3    2    1    0
     * | Pr | Privl | 1  | Ex | DC | RW | Ac |
     *
     * Pr: "Present Bit". Set to 1 to signify selector is valid.
     * Privl: Privilege. Ring level (0-3).
     * Ex: Executable bit. 1 for code segment, 0 for data.
     * DC: Direction bit/Conforming bit.
     *     - Direction bit for data selectors: Tells the direction.
     *         * 0 the segment grows up.
     *         * 1 the segment grows down, ie. the offset has to be greater than the limit.
     *     - Conforming bit for code selectors:
     *         * If 1 code in this segment can be executed from an equal or lower privilege level. For example, code in 
     *           ring 3 can far-jump to conforming code in a ring 2 segment. The privl-bits represent the highest 
     *           privilege level that is allowed to execute the segment. For example, code in ring 0 cannot far-jump to
     *           a conforming code segment with privl==0x2, while code in ring 2 and 3 can. Note that the privilege 
     *           level remains the same, ie. a far-jump form ring 3 to a privl==2-segment remains in ring 3 after the 
     *           jump.
     *         * If 0 code in this segment can only be executed from the ring set in privl.
     * RW: Readable bit/Writable bit.
     *     - Readable bit for code selectors: Whether read access for this segment is allowed. Write access is never
     *       allowed for code segments.
     *     - Writable bit for data selectors: Whether write access for this segment is allowed. Read access is always 
     *       allowed for data segments.
     * Ac: Accessed bit. Just set to 0. The CPU sets this to 1 when the segment is accessed.
     */
    enum class Type : uint8_t
    {
        None       = 0x0,  // blank
        KernelCode = 0x9A, // Ring 0, executable-read
        KernelData = 0x92, // Ring 0, read/write
        UserCode   = 0xFA, // Ring 3, executable-read
        UserData   = 0xF2, // Ring 3, read/write
        TSS        = 0x89  // I dunno man, I just work here.
    };

    GDTEntry() : _base(0), _limit(0), _type(0) {}

    GDTEntry(uint32_t base, uint32_t limit, Type type) : _base(base), _limit(limit), _type((uint8_t)type) {}

    friend class GlobalDescriptorTable;

private:
    uint32_t _base;
    uint32_t _limit;
    uint8_t _type;
};

class GlobalDescriptorTable
{
public:
    void encodeEntry(uint8_t entryNumber, GDTEntry source);

    void install() { set_gdt(this->gdt, sizeof(uint64_t) * GDT_SIZE); }

    void installTSS(uint8_t entryIndex);

private:
    uint64_t gdt[GDT_SIZE];
};

typedef struct tss_entry
{
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

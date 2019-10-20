#include <arch/i386/cpu/GlobalDescriptorTable.hpp>

//======================================================
// GlobalDescriptorTable
//======================================================
void GlobalDescriptorTable::encodeEntry(uint8_t entryNumber, GDTEntry source)
{
    uint8_t *target = (uint8_t*)&(this->gdt[entryNumber]);

    if (source._limit > 65536) {
        // Adjust granularity if required
        source._limit = source._limit >> 12;
        target[6] = 0xC0;
    } else {
        target[6] = 0x40;
    }

    // Encode the limit
    target[0] = static_cast<uint8_t>(source._limit & 0xFFu);
    target[1] = static_cast<uint8_t>((source._limit >> 8u) & 0xFFu);
    target[6] |= static_cast<uint8_t>((source._limit >> 16u) & 0xFu);

    // Encode the base 
    target[2] = static_cast<uint8_t>(source._base & 0xFFu);
    target[3] = static_cast<uint8_t>((source._base >> 8u) & 0xFFu);
    target[4] = static_cast<uint8_t>((source._base >> 16u) & 0xFFu);
    target[7] = static_cast<uint8_t>((source._base >> 24u) & 0xFFu);

    // And... Type
    target[5] = source._type;
}

void GlobalDescriptorTable::installTSS(uint8_t entryIndex)
{
    uint32_t descriptor = static_cast<uint32_t>(entryIndex) * 8u;
    asm volatile ("movl %0, %%eax;"
                  "ltr %%ax;" : : "r"(descriptor) : "%eax");
}

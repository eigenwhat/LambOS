#include <kernel/InterruptDescriptorTable.hpp>

//======================================================
// InterruptDescriptorTable
//======================================================

void InterruptDescriptorTable::encodeEntry(uint8_t entryNumber, IDTEntry source)
{
	uint8_t *target = (uint8_t*)&(this->idt[entryNumber]);

	// offset
	target[0] = source.offset & 0xFF;
	target[1] = (source.offset >> 8) & 0xFF;
   	target[6] = (source.offset >> 16) & 0xFF;
   	target[7] = (source.offset >> 24) & 0xFF;

   	// a code segment selector in GDT or LDT
   	target[2] = source.selector & 0xFF;
   	target[3] = (source.selector >> 8) & 0xFF;

   	// unused, set to 0
   	target[4] = 0;

   	// type and attributes
   	target[5] = source.type_attr;
}

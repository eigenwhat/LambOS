#include "InterruptDescriptorTable.hpp"
#include "isr.h"

#define ADDISR(num) do { \
      encodeEntry((num), IDTEntry(0x08, (uint32_t)isr##num, 0x8E));\
      isr[(num)] = defaultISR;\
   } while(0)

extern "C" void defaultISR(RegisterTable &registers) {}

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

void InterruptDescriptorTable::encodeHWExceptionISRs()
{
   ADDISR(0);
   ADDISR(1);
   ADDISR(2);
   ADDISR(3);
   ADDISR(4);
   ADDISR(5);
   ADDISR(6);
   ADDISR(7);
   ADDISR(8);
   ADDISR(9);
   ADDISR(10);
   ADDISR(11);
   ADDISR(12);
   ADDISR(13);
   ADDISR(14);
   ADDISR(15);
   ADDISR(16);
   ADDISR(17);
   ADDISR(18);
   ADDISR(19);
   ADDISR(20);
   ADDISR(21);
   ADDISR(22);
   ADDISR(23);
   ADDISR(24);
   ADDISR(25);
   ADDISR(26);
   ADDISR(27);
   ADDISR(28);
   ADDISR(29);
   ADDISR(30);
   ADDISR(31);
}

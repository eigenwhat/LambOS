#include <stdlib.h>
#include <new>
#include <sys/asm.h>
#include <io/BochsDebugOutputStream.hpp>
#include <cpu/InterruptDescriptorTable.hpp>
#include "isr.h"

#define ADDISR(num, isrf) do { \
      encodeEntry((num), IDTEntry(0x08, (uint32_t)isr##num, 0x8E));\
      isr[(num)] = (isrf);\
   } while(0)

struct StubISR : InterruptServiceRoutine
{
   StubISR(OutputStream &out) : _out(out) {}
   virtual void operator()(RegisterTable &registers) 
   {
      char hexval[33];
      _out.write((uint8_t*)"int 0x", 6);
      itoa(registers.int_no, hexval, 16);
      _out.write((uint8_t*)hexval, 2);
      _out.write((uint8_t*)", err 0x", 8);
      itoa(registers.err_code, hexval, 16);
      _out.write((uint8_t*)hexval, 2);
      _out.write('\n');
   }
private:
   OutputStream &_out;
};

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

uint8_t isr_mem[sizeof(StubISR)];
uint8_t bochs_out_mem[sizeof(BochsDebugOutputStream)];

void InterruptDescriptorTable::encodeHWExceptionISRs()
{
   BochsDebugOutputStream *out = new (bochs_out_mem) BochsDebugOutputStream;
   StubISR *defaultISR = new (isr_mem) StubISR(*out);
   ADDISR(0, defaultISR);
   ADDISR(1, defaultISR);
   ADDISR(2, defaultISR);
   ADDISR(3, defaultISR);
   ADDISR(4, defaultISR);
   ADDISR(5, defaultISR);
   ADDISR(6, defaultISR);
   ADDISR(7, defaultISR);
   ADDISR(8, defaultISR);
   ADDISR(9, defaultISR);
   ADDISR(10, defaultISR);
   ADDISR(11, defaultISR);
   ADDISR(12, defaultISR);
   ADDISR(13, defaultISR);
   ADDISR(14, defaultISR);
   ADDISR(15, defaultISR);
   ADDISR(16, defaultISR);
   ADDISR(17, defaultISR);
   ADDISR(18, defaultISR);
   ADDISR(19, defaultISR);
   ADDISR(20, defaultISR);
   ADDISR(21, defaultISR);
   ADDISR(22, defaultISR);
   ADDISR(23, defaultISR);
   ADDISR(24, defaultISR);
   ADDISR(25, defaultISR);
   ADDISR(26, defaultISR);
   ADDISR(27, defaultISR);
   ADDISR(28, defaultISR);
   ADDISR(29, defaultISR);
   ADDISR(30, defaultISR);
   ADDISR(31, defaultISR);
}

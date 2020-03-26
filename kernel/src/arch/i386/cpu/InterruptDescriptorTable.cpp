//
// Created by Martin Miralles-Cordal on 8/12/2013.
//

#include <arch/i386/cpu/InterruptDescriptorTable.hpp>
#include <arch/i386/cpu/X86.hpp>
#include <arch/i386/X86Kernel.hpp>

#include <io/Print.hpp>
#include <system/asm.h>
#include <system/Debug.hpp>

#include <ranges>
#include <cstdio>
#include <cstdlib>
#include <new>
#include <cstring>

#include "isr.h"

static constexpr const char *exception_messages[32] = {
        "Division by zero",
        "Debug",
        "Non-maskable interrupt",
        "Breakpoint",
        "Detected overflow",
        "Out-of-bounds",
        "Invalid opcode",
        "No coprocessor",
        "Double fault",
        "Coprocessor segment overrun",
        "Bad TSS",
        "Segment not present",
        "Stack fault",
        "General protection fault",
        "Page fault",
        "Unknown interrupt",
        "Coprocessor fault",
        "Alignment check",
        "Machine check",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved"
};

//======================================================
// ISR functions
//======================================================

namespace {

inline std::byte operator""_B(unsigned long long v) { return static_cast<std::byte>(v); }


inline void addISR(InterruptDescriptorTable &idt, std::uint8_t num, InterruptServiceRoutine *isrf)
{
    idt.encodeEntry(num, IDTEntry(0x08, kIsrs[num], 0x8E));
    idt.setISR(InterruptNumber{num}, isrf);
}

}

class StubISR : public InterruptServiceRoutine
{
  public:
    StubISR(sys::OutputStream &out) : _out(out) {}

    virtual void operator()(RegisterTable &registers)
    {
        sys::println(_out, "==== LAMBOS ISR CALLED ====");
        sys::println(_out, "%@ (%@): err %@", exception_messages[registers.int_no],
                                              uint32_t{registers.int_no}, uint32_t{registers.err_code});
        sys::println(_out, "==== CARRY ON, CITIZEN ====");
    }

private:
    sys::OutputStream &_out;
};

class PanicISR : public InterruptServiceRoutine
{
  public:
    virtual void operator()(RegisterTable &registers)
    {
        kernel->panic(exception_messages[registers.int_no]);
    }
};

extern "C" void interrupt_handler(RegisterTable registers)
{
    auto &cpu = static_cast<X86Kernel*>(kernel)->cpu();
    cpu.idt()->callISR(static_cast<InterruptNumber>(registers.int_no), registers);
}

//======================================================
// InterruptDescriptorTable
//======================================================

std::uint64_t InterruptDescriptorTable::EncodeEntry(IDTEntry source)
{
    std::byte target[8];

    // offset
    target[0] = static_cast<std::byte>(source._offset & 0xFFu);
    target[1] = static_cast<std::byte>((source._offset >> 8u) & 0xFFu);
    target[6] = static_cast<std::byte>((source._offset >> 16u) & 0xFFu);
    target[7] = static_cast<std::byte>((source._offset >> 24u) & 0xFFu);

    // a code segment selector in GDT or LDT
    target[2] = static_cast<std::byte>(source._selector & 0xFFu);
    target[3] = static_cast<std::byte>(source._selector >> 8u) & 0xFF_B;

    // unused, set to 0
    target[4] = 0_B;

    // type and attributes
    target[5] = std::byte(source._typeAttr);

    std::uint64_t result;
    std::memcpy(&result, target, sizeof(target));
    return result;
}

void InterruptDescriptorTable::encodeEntry(uint8_t entryNumber, IDTEntry source)
{
    idt_[entryNumber] = EncodeEntry(source);
}

uint8_t stub_isr_mem[sizeof(StubISR)];
uint8_t panic_isr_mem[sizeof(PanicISR)];

void InterruptDescriptorTable::encodeISRs()
{
    StubISR *defaultISR = new(stub_isr_mem) StubISR(debugOut);
    PanicISR *panicISR = new(panic_isr_mem) PanicISR;

    // assign all the defaults
    std::ranges::generate_n(idt_, 256, [] { static int i = 0; return EncodeEntry(IDTEntry(0x08, kIsrs[i++], 0x8E)); });
    std::ranges::fill_n(isr_, 256, defaultISR);

    // configure system interrupts
    addISR(*this, 0,  defaultISR); // kDivideByZero
    addISR(*this, 1,  defaultISR); // kDebugger
    addISR(*this, 2,  defaultISR); // kNMI
    addISR(*this, 3,  defaultISR); // kBreakpoint
    addISR(*this, 4,  defaultISR); // kOverflow
    addISR(*this, 5,  defaultISR); // kBounds
    addISR(*this, 6,  defaultISR); // kInvalidOpcode
    addISR(*this, 7,  defaultISR); // kCoprocessorNotAvailable
    addISR(*this, 8,  panicISR);   // kDoubleFault
    addISR(*this, 9,  defaultISR); // kCoprocessorSegmentOverrun
    addISR(*this, 10, defaultISR); // kInvalidTSS
    addISR(*this, 11, defaultISR); // kSegmentNotPresent
    addISR(*this, 12, defaultISR); // kStackFault
    addISR(*this, 13, defaultISR); // kGeneralProtectionFault
    addISR(*this, 14, defaultISR); // kPageFault
    addISR(*this, 15, defaultISR);
    addISR(*this, 16, defaultISR); // kMathFault
    addISR(*this, 17, defaultISR); // kAlignmentCheck
    addISR(*this, 18, defaultISR); // kMachineCheck
    addISR(*this, 19, defaultISR); // kSIMDFloatingPointException
}

//
// Created by Martin Miralles-Cordal on 8/12/2013.
//

#include <arch/i386/cpu/InterruptDescriptorTable.hpp>
#include <arch/i386/cpu/X86.hpp>
#include <arch/i386/sys/asm.h>
#include <arch/i386/X86Kernel.hpp>

#include <io/debug.h>

#include <cstdio>
#include <cstdlib>
#include <new>

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

#define ADDISR(num, isrf) do { \
      encodeEntry((num), IDTEntry(0x08, (uint32_t)ISR_FUNCTION_NAME(num), 0x8E));\
      isr[(num)] = (isrf);\
   } while(0)

class StubISR : public InterruptServiceRoutine
{
  public:
    StubISR(sys::PrintStream &out) : _out(out) {}

    virtual void operator()(RegisterTable &registers)
    {
        _out.println("==== LAMBOS ISR CALLED ====");
        _out.print(exception_messages[registers.int_no]);
        char message[33];
        sprintf(message, " (%x): err %x", registers.int_no, registers.err_code);
        _out.println(message);
        _out.println("==== CARRY ON, CITIZEN ====");
    }

private:
    sys::PrintStream &_out;
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

void InterruptDescriptorTable::encodeEntry(uint8_t entryNumber, IDTEntry source)
{
    uint8_t *target = (uint8_t * ) & (this->idt[entryNumber]);

    // offset
    target[0] = static_cast<std::uint8_t>(source._offset & 0xFFu);
    target[1] = static_cast<std::uint8_t>((source._offset >> 8u) & 0xFFu);
    target[6] = static_cast<std::uint8_t>((source._offset >> 16u) & 0xFFu);
    target[7] = static_cast<std::uint8_t>((source._offset >> 24u) & 0xFFu);

    // a code segment selector in GDT or LDT
    target[2] = static_cast<std::uint8_t>(source._selector & 0xFFu);
    target[3] = static_cast<std::uint8_t>(source._selector >> 8u) & 0xFFu;

    // unused, set to 0
    target[4] = 0;

    // type and attributes
    target[5] = source._typeAttr;
}

uint8_t stub_isr_mem[sizeof(StubISR)];
uint8_t panic_isr_mem[sizeof(PanicISR)];

void InterruptDescriptorTable::encodeISRs()
{
    StubISR *defaultISR = new(stub_isr_mem) StubISR(*debugOut);
    PanicISR *panicISR = new(panic_isr_mem) PanicISR;

    ADDISR(0, defaultISR);  // kDivideByZero
    ADDISR(1, defaultISR);  // kDebugger
    ADDISR(2, defaultISR);  // kNMI
    ADDISR(3, defaultISR);  // kBreakpoint
    ADDISR(4, defaultISR);  // kOverflow
    ADDISR(5, defaultISR);  // kBounds
    ADDISR(6, defaultISR);  // kInvalidOpcode
    ADDISR(7, defaultISR);  // kCoprocessorNotAvailable
    ADDISR(8, panicISR);    // kDoubleFault
    ADDISR(9, defaultISR);  // kCoprocessorSegmentOverrun
    ADDISR(10, defaultISR); // kInvalidTSS
    ADDISR(11, defaultISR); // kSegmentNotPresent
    ADDISR(12, defaultISR); // kStackFault
    ADDISR(13, defaultISR); // kGeneralProtectionFault
    ADDISR(14, defaultISR); // kPageFault
    ADDISR(15, defaultISR);
    ADDISR(16, defaultISR); // kMathFault
    ADDISR(17, defaultISR); // kAlignmentCheck
    ADDISR(18, defaultISR); // kMachineCheck
    ADDISR(19, defaultISR); // kSIMDFloatingPointException
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
    ADDISR(32, defaultISR);
    ADDISR(33, defaultISR);
    ADDISR(34, defaultISR);
    ADDISR(35, defaultISR);
    ADDISR(36, defaultISR);
    ADDISR(37, defaultISR);
    ADDISR(38, defaultISR);
    ADDISR(39, defaultISR);
    ADDISR(40, defaultISR);
    ADDISR(41, defaultISR);
    ADDISR(42, defaultISR);
    ADDISR(43, defaultISR);
    ADDISR(44, defaultISR);
    ADDISR(45, defaultISR);
    ADDISR(46, defaultISR);
    ADDISR(47, defaultISR);
    ADDISR(48, defaultISR);
    ADDISR(49, defaultISR);
    ADDISR(50, defaultISR);
    ADDISR(51, defaultISR);
    ADDISR(52, defaultISR);
    ADDISR(53, defaultISR);
    ADDISR(54, defaultISR);
    ADDISR(55, defaultISR);
    ADDISR(56, defaultISR);
    ADDISR(57, defaultISR);
    ADDISR(58, defaultISR);
    ADDISR(59, defaultISR);
    ADDISR(60, defaultISR);
    ADDISR(61, defaultISR);
    ADDISR(62, defaultISR);
    ADDISR(63, defaultISR);
    ADDISR(64, defaultISR);
    ADDISR(65, defaultISR);
    ADDISR(66, defaultISR);
    ADDISR(67, defaultISR);
    ADDISR(68, defaultISR);
    ADDISR(69, defaultISR);
    ADDISR(70, defaultISR);
    ADDISR(71, defaultISR);
    ADDISR(72, defaultISR);
    ADDISR(73, defaultISR);
    ADDISR(74, defaultISR);
    ADDISR(75, defaultISR);
    ADDISR(76, defaultISR);
    ADDISR(77, defaultISR);
    ADDISR(78, defaultISR);
    ADDISR(79, defaultISR);
    ADDISR(80, defaultISR);
    ADDISR(81, defaultISR);
    ADDISR(82, defaultISR);
    ADDISR(83, defaultISR);
    ADDISR(84, defaultISR);
    ADDISR(85, defaultISR);
    ADDISR(86, defaultISR);
    ADDISR(87, defaultISR);
    ADDISR(88, defaultISR);
    ADDISR(89, defaultISR);
    ADDISR(90, defaultISR);
    ADDISR(91, defaultISR);
    ADDISR(92, defaultISR);
    ADDISR(93, defaultISR);
    ADDISR(94, defaultISR);
    ADDISR(95, defaultISR);
    ADDISR(96, defaultISR);
    ADDISR(97, defaultISR);
    ADDISR(98, defaultISR);
    ADDISR(99, defaultISR);
    ADDISR(100, defaultISR);
    ADDISR(101, defaultISR);
    ADDISR(102, defaultISR);
    ADDISR(103, defaultISR);
    ADDISR(104, defaultISR);
    ADDISR(105, defaultISR);
    ADDISR(106, defaultISR);
    ADDISR(107, defaultISR);
    ADDISR(108, defaultISR);
    ADDISR(109, defaultISR);
    ADDISR(110, defaultISR);
    ADDISR(111, defaultISR);
    ADDISR(112, defaultISR);
    ADDISR(113, defaultISR);
    ADDISR(114, defaultISR);
    ADDISR(115, defaultISR);
    ADDISR(116, defaultISR);
    ADDISR(117, defaultISR);
    ADDISR(118, defaultISR);
    ADDISR(119, defaultISR);
    ADDISR(120, defaultISR);
    ADDISR(121, defaultISR);
    ADDISR(122, defaultISR);
    ADDISR(123, defaultISR);
    ADDISR(124, defaultISR);
    ADDISR(125, defaultISR);
    ADDISR(126, defaultISR);
    ADDISR(127, defaultISR);
    ADDISR(128, defaultISR);
    ADDISR(129, defaultISR);
    ADDISR(130, defaultISR);
    ADDISR(131, defaultISR);
    ADDISR(132, defaultISR);
    ADDISR(133, defaultISR);
    ADDISR(134, defaultISR);
    ADDISR(135, defaultISR);
    ADDISR(136, defaultISR);
    ADDISR(137, defaultISR);
    ADDISR(138, defaultISR);
    ADDISR(139, defaultISR);
    ADDISR(140, defaultISR);
    ADDISR(141, defaultISR);
    ADDISR(142, defaultISR);
    ADDISR(143, defaultISR);
    ADDISR(144, defaultISR);
    ADDISR(145, defaultISR);
    ADDISR(146, defaultISR);
    ADDISR(147, defaultISR);
    ADDISR(148, defaultISR);
    ADDISR(149, defaultISR);
    ADDISR(150, defaultISR);
    ADDISR(151, defaultISR);
    ADDISR(152, defaultISR);
    ADDISR(153, defaultISR);
    ADDISR(154, defaultISR);
    ADDISR(155, defaultISR);
    ADDISR(156, defaultISR);
    ADDISR(157, defaultISR);
    ADDISR(158, defaultISR);
    ADDISR(159, defaultISR);
    ADDISR(160, defaultISR);
    ADDISR(161, defaultISR);
    ADDISR(162, defaultISR);
    ADDISR(163, defaultISR);
    ADDISR(164, defaultISR);
    ADDISR(165, defaultISR);
    ADDISR(166, defaultISR);
    ADDISR(167, defaultISR);
    ADDISR(168, defaultISR);
    ADDISR(169, defaultISR);
    ADDISR(170, defaultISR);
    ADDISR(171, defaultISR);
    ADDISR(172, defaultISR);
    ADDISR(173, defaultISR);
    ADDISR(174, defaultISR);
    ADDISR(175, defaultISR);
    ADDISR(176, defaultISR);
    ADDISR(177, defaultISR);
    ADDISR(178, defaultISR);
    ADDISR(179, defaultISR);
    ADDISR(180, defaultISR);
    ADDISR(181, defaultISR);
    ADDISR(182, defaultISR);
    ADDISR(183, defaultISR);
    ADDISR(184, defaultISR);
    ADDISR(185, defaultISR);
    ADDISR(186, defaultISR);
    ADDISR(187, defaultISR);
    ADDISR(188, defaultISR);
    ADDISR(189, defaultISR);
    ADDISR(190, defaultISR);
    ADDISR(191, defaultISR);
    ADDISR(192, defaultISR);
    ADDISR(193, defaultISR);
    ADDISR(194, defaultISR);
    ADDISR(195, defaultISR);
    ADDISR(196, defaultISR);
    ADDISR(197, defaultISR);
    ADDISR(198, defaultISR);
    ADDISR(199, defaultISR);
    ADDISR(200, defaultISR);
    ADDISR(201, defaultISR);
    ADDISR(202, defaultISR);
    ADDISR(203, defaultISR);
    ADDISR(204, defaultISR);
    ADDISR(205, defaultISR);
    ADDISR(206, defaultISR);
    ADDISR(207, defaultISR);
    ADDISR(208, defaultISR);
    ADDISR(209, defaultISR);
    ADDISR(210, defaultISR);
    ADDISR(211, defaultISR);
    ADDISR(212, defaultISR);
    ADDISR(213, defaultISR);
    ADDISR(214, defaultISR);
    ADDISR(215, defaultISR);
    ADDISR(216, defaultISR);
    ADDISR(217, defaultISR);
    ADDISR(218, defaultISR);
    ADDISR(219, defaultISR);
    ADDISR(220, defaultISR);
    ADDISR(221, defaultISR);
    ADDISR(222, defaultISR);
    ADDISR(223, defaultISR);
    ADDISR(224, defaultISR);
    ADDISR(225, defaultISR);
    ADDISR(226, defaultISR);
    ADDISR(227, defaultISR);
    ADDISR(228, defaultISR);
    ADDISR(229, defaultISR);
    ADDISR(230, defaultISR);
    ADDISR(231, defaultISR);
    ADDISR(232, defaultISR);
    ADDISR(233, defaultISR);
    ADDISR(234, defaultISR);
    ADDISR(235, defaultISR);
    ADDISR(236, defaultISR);
    ADDISR(237, defaultISR);
    ADDISR(238, defaultISR);
    ADDISR(239, defaultISR);
    ADDISR(240, defaultISR);
    ADDISR(241, defaultISR);
    ADDISR(242, defaultISR);
    ADDISR(243, defaultISR);
    ADDISR(244, defaultISR);
    ADDISR(245, defaultISR);
    ADDISR(246, defaultISR);
    ADDISR(247, defaultISR);
    ADDISR(248, defaultISR);
    ADDISR(249, defaultISR);
    ADDISR(250, defaultISR);
    ADDISR(251, defaultISR);
    ADDISR(252, defaultISR);
    ADDISR(253, defaultISR);
    ADDISR(254, defaultISR);
    ADDISR(255, defaultISR);
}

#ifndef __INTDEFS_H
#define __INTDEFS_H

#include <decl.h>

__BEGIN_DECLS

typedef enum interrupt_number {
    kIntDivideByZero = 0,
    kIntDebugger,
    kIntNMI,
    kIntBreakpoint,
    kIntOverflow,
    kIntBounds,
    kIntInvalidOpcode,
    kIntCoprocessorNotAvailable,
    kIntDoubleFault,
    kIntCoprocessorSegmentOverrun,
    kIntInvalidTSS,
    kIntSegmentNotPresent,
    kIntStackFault,
    kIntGeneralProtectionFault,
    kIntPageFault,
    kInt0E,
    kIntMathFault,
    kIntAlignmentCheck,
    kIntMachineCheck,
    kIntSIMDFloatingPointException,
    kIntIRQ0 = 32,
    kIntKeyboardIRQ,
    kIntIRQ2,
    kIntIRQ3,
    kIntIRQ4,
    kIntIRQ5,
    kIntIRQ6,
    kIntIRQ7,
    kIntIRQ8,
    kIntIRQ9,
    kIntIRQ10,
    kIntIRQ11,
    kIntIRQ12,
    kIntIRQ13,
    kIntIRQ14,
    kIntIRQ15
} InterruptNumber;

__END_DECLS

#endif

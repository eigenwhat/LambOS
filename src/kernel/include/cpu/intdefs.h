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
    kIntSIMDFloatingPointException    
} InterruptNumber;

__END_DECLS

#endif

cmake_minimum_required(VERSION 3.5)
project(lambos VERSION 0.1 LANGUAGES C CXX ASM-ATT)

set(KERNEL_TARGET lambos-kernel)
set(KERNEL_TARGET ${KERNEL_TARGET} PARENT_SCOPE)

set(I386_SOURCES
    src/arch/i386/cpu/GlobalDescriptorTable.cpp
    src/arch/i386/cpu/InterruptDescriptorTable.cpp
    src/arch/i386/cpu/X86CPU.cpp
    src/arch/i386/cpu/X86RealTimeClock.cpp
    src/arch/i386/cpu/boot.s
    src/arch/i386/cpu/interrupt.s
    src/arch/i386/cpu/set_gdt.s
    src/arch/i386/cpu/set_idt.s
    src/arch/i386/device/display/VGATextConsole.cpp
    src/arch/i386/device/input/KeyboardInputStream.cpp
    src/arch/i386/device/input/PS2Keyboard.cpp
    src/arch/i386/mem/MMU.cpp
    src/arch/i386/mem/PageFrameAllocator.cpp
    src/arch/i386/mem/PageTable.cpp
    src/arch/i386/main.cpp)

set (LIBALLOC_SOURCES
    src/mem/liballoc.c
    src/mem/liballoc_hook.cpp)

set (SOURCES
    ${I386_SOURCES}
    ${LIBALLOC_SOURCES}
    src/Kernel.cpp)

add_executable(${KERNEL_TARGET} ${SOURCES})
target_include_directories(${KERNEL_TARGET} PRIVATE include ../libsys/include)
target_link_libraries(${KERNEL_TARGET} ${KERNEL_HEADERS_TARGET})
set_property(TARGET ${KERNEL_TARGET} PROPERTY OUTPUT_NAME kernel.bin)

# set compilation options
set_property(TARGET ${KERNEL_TARGET} PROPERTY CXX_STANDARD 11)
target_compile_options(${KERNEL_TARGET} PRIVATE
        $<$<COMPILE_LANGUAGE:C>:${C_COMPILER_FLAGS}> $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILER_FLAGS}>)

# check for and link support libs
if (NOT DEFINED LIBC_TARGET)
    message (SEND_ERROR "LIBC_TARGET not defined or not visible to ${KERNEL_TARGET}.")
else ()
    target_link_libraries(${KERNEL_TARGET} ${LIBC_TARGET})
endif ()

if (NOT DEFINED LIBCPP_TARGET)
    message (SEND_ERROR "LIBCPP_TARGET not defined or not visible to ${KERNEL_TARGET}.")
else ()
    target_link_libraries(${KERNEL_TARGET} ${LIBCPP_TARGET})
endif ()

if (NOT DEFINED LIBSYS_TARGET)
    message (SEND_ERROR "LIBSYS_TARGET not defined or not visible to ${KERNEL_TARGET}.")
else ()
    target_link_libraries(${KERNEL_TARGET} ${LIBSYS_TARGET})
endif()

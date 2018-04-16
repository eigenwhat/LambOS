cmake_minimum_required(VERSION 3.5)
project(lambos)

if (NOT DEFINED KERNEL_TARGET)
    set(KERNEL_TARGET lambos-kernel)
    message (WARNING "KERNEL_TARGET was not set by project root. Defaulting to '${KERNEL_TARGET}'.")
    set(KERNEL_TARGET ${KERNEL_TARGET} PARENT_SCOPE)
endif ()

file(GLOB_RECURSE INCLUDE_FILES include/*.h include/*.hpp)

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
        src/arch/i386/mem/MMU.cpp
        src/arch/i386/mem/PageFrameAllocator.cpp
        src/arch/i386/mem/PageTable.cpp
        src/arch/i386/X86Kernel.cpp
        src/arch/i386/main.cpp)

set (LIBALLOC_SOURCES
        src/mem/liballoc.c
        src/mem/liballoc_hook.cpp)

set (SOURCES
        ${I386_SOURCES}
        ${LIBALLOC_SOURCES}
        src/device/input/KeyboardInputStream.cpp
        src/device/input/PS2Keyboard.cpp
        src/Kernel.cpp)

add_executable(${KERNEL_TARGET} ${SOURCES} ${INCLUDE_FILES})
target_include_directories(${KERNEL_TARGET} PUBLIC include)
target_include_directories(${KERNEL_TARGET} INTERFACE $<TARGET_PROPERTY:${LIBSYS_TARGET},INTERFACE_INCLUDE_DIRECTORIES>)
#target_link_libraries(${KERNEL_TARGET} ${KERNEL_HEADERS_TARGET})
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

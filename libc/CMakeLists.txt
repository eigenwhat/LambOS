cmake_minimum_required(VERSION 3.5)
project(lambos)

if (NOT DEFINED LIBC_TARGET)
    set(LIBC_TARGET lambos-libc)
    message (WARNING "LIBC_TARGET was not set by project root. Defaulting to '${LIBC_TARGET}'.")
    set(LIBC_TARGET ${LIBC_TARGET} PARENT_SCOPE)
endif ()

set (STDIO_SOURCES
    src/stdio/printf.c
    src/stdio/putchar.cpp)

set (STDLIB_SOURCES
    src/stdlib/itoa.c)

set (SOURCES
    ${STDIO_SOURCES}
    ${STDLIB_SOURCES}
    src/ctype.c
    src/string.c)

add_library(${LIBC_TARGET} STATIC ${SOURCES})
target_include_directories(${LIBC_TARGET} PUBLIC include)
target_include_directories(${LIBC_TARGET} PRIVATE $<TARGET_PROPERTY:${KERNEL_TARGET},INTERFACE_INCLUDE_DIRECTORIES>)
set_property(TARGET ${LIBC_TARGET} PROPERTY CXX_STANDARD 17)
target_compile_options(${LIBC_TARGET} PRIVATE
    $<$<COMPILE_LANGUAGE:C>:${C_COMPILER_FLAGS}> $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILER_FLAGS}>)
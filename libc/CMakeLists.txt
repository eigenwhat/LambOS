cmake_minimum_required(VERSION 3.5)
project(lambos-libc VERSION 0.1)

if (NOT DEFINED LIBC_TARGET)
    message (WARNING "LIBC_TARGET not defined in project root. Defining it now...")
    set(LIBC_TARGET lambos-libc)
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
set_property(TARGET ${LIBC_TARGET} PROPERTY CXX_STANDARD 11)
target_compile_options(${LIBC_TARGET} PRIVATE
    $<$<COMPILE_LANGUAGE:C>:${C_COMPILER_FLAGS}> $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILER_FLAGS}>)

target_link_libraries(${LIBC_TARGET} ${KERNEL_HEADERS_TARGET})
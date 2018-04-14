cmake_minimum_required(VERSION 3.5)
project(libc VERSION 0.1)

set(LIBC_LIBRARY libc)
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

add_library(${LIBC_LIBRARY} STATIC ${SOURCES})
target_include_directories(${LIBC_LIBRARY} PUBLIC include)
set_property(TARGET ${LIBC_LIBRARY} PROPERTY CXX_STANDARD 11)
target_compile_options(${LIBC_LIBRARY} PRIVATE
    -ffreestanding -O2 -Wall -Wextra -Werror -fstack-protector-all
    $<$<LANG:C>:-std=gnu99>
    $<$<LANG:CXX>:-fno-exceptions -fno-rtti>)

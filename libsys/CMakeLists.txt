cmake_minimum_required(VERSION 3.5)
project(lambos)

if (NOT DEFINED LIBSYS_TARGET)
    message (WARNING "LIBSYS_TARGET not defined in project root. Defining it now...")
    set(LIBSYS_TARGET lambos-libsys)
    set(LIBSYS_TARGET ${LIBSYS_TARGET} PARENT_SCOPE)
endif ()

set (SOURCES
        src/io/PrintStream.cpp)

add_library(${LIBSYS_TARGET} STATIC ${SOURCES})
target_include_directories(${LIBSYS_TARGET} PUBLIC include)
set_property(TARGET ${LIBSYS_TARGET} PROPERTY CXX_STANDARD 11)
target_compile_options(${LIBSYS_TARGET} PRIVATE
        $<$<COMPILE_LANGUAGE:C>:${C_COMPILER_FLAGS}> $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILER_FLAGS}>)
target_link_libraries(${LIBSYS_TARGET} ${LIBC_TARGET})
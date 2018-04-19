cmake_minimum_required(VERSION 3.8)
project(lambos)

if (NOT DEFINED LIBSYS_TARGET)
    set(LIBSYS_TARGET lambos-libsys)
    message (WARNING "LIBSYS_TARGET was not set by project root. Defaulting to '${LIBSYS_TARGET}'.")
    set(LIBSYS_TARGET ${LIBSYS_TARGET} PARENT_SCOPE)
endif ()

set (SOURCES
        src/io/PrintStream.cpp)

add_library(${LIBSYS_TARGET} STATIC ${SOURCES})
target_include_directories(${LIBSYS_TARGET} PUBLIC include)
set_property(TARGET ${LIBSYS_TARGET} PROPERTY CXX_STANDARD 17)
target_compile_options(${LIBSYS_TARGET} PRIVATE
        $<$<COMPILE_LANGUAGE:C>:${C_COMPILER_FLAGS}> $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILER_FLAGS}>)
target_link_libraries(${LIBSYS_TARGET} ${LIBC_TARGET})
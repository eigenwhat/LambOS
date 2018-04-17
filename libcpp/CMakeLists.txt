cmake_minimum_required(VERSION 3.5)
project(lambos)

if (NOT DEFINED LIBCPP_TARGET)
    set(LIBCPP_TARGET lambos-libcpp)
    message (WARNING "LIBCPP_TARGET was not set by project root. Defaulting to '${LIBCPP_TARGET}'.")
    set(LIBCPP_TARGET ${LIBCPP_TARGET} PARENT_SCOPE)
endif ()

set (INCLUDE_FILES
        include/cstring include/cstdio)
set (SOURCES
        src/_lang.cpp)

add_library(${LIBCPP_TARGET} STATIC ${SOURCES} ${INCLUDE_FILES})
target_include_directories(${LIBCPP_TARGET} PUBLIC include)
target_include_directories(${LIBCPP_TARGET} PRIVATE $<TARGET_PROPERTY:${KERNEL_TARGET},INTERFACE_INCLUDE_DIRECTORIES>)
set_property(TARGET ${LIBCPP_TARGET} PROPERTY CXX_STANDARD 11)
target_compile_options(${LIBCPP_TARGET} PRIVATE
        $<$<COMPILE_LANGUAGE:C>:${C_COMPILER_FLAGS}> $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILER_FLAGS}>)
target_link_libraries(${LIBCPP_TARGET} PUBLIC ${LIBC_TARGET})
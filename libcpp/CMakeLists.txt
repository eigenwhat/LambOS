cmake_minimum_required(VERSION 3.5)
project(lambos-libcpp VERSION 0.1)

if (NOT DEFINED LIBCPP_TARGET)
    message (WARNING "LIBCPP_TARGET not defined in project root. Defining it now...")
    set(LIBCPP_TARGET lambos-libcpp)
    set(LIBCPP_TARGET ${LIBCPP_TARGET} PARENT_SCOPE)
endif ()

set (SOURCES
    src/_lang.cpp)

add_library(${LIBCPP_TARGET} STATIC ${SOURCES})
target_include_directories(${LIBCPP_TARGET} PUBLIC include)
set_property(TARGET ${LIBCPP_TARGET} PROPERTY CXX_STANDARD 11)
target_compile_options(${LIBCPP_TARGET} PRIVATE
        $<$<COMPILE_LANGUAGE:C>:${C_COMPILER_FLAGS}> $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILER_FLAGS}>)
target_link_libraries(${LIBCPP_TARGET} ${KERNEL_HEADERS_TARGET})

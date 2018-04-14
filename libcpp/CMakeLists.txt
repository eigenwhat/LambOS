cmake_minimum_required(VERSION 3.5)
project(libcpp VERSION 0.1)

set(LIBCPP_LIBRARY libcpp)
set (SOURCES
    src/_lang.cpp)

add_library(${LIBCPP_LIBRARY} STATIC ${SOURCES})
target_include_directories(${LIBCPP_LIBRARY} PUBLIC include)
set_property(TARGET ${LIBCPP_LIBRARY} PROPERTY CXX_STANDARD 11)
target_compile_options(${LIBCPP_LIBRARY} PRIVATE
    -ffreestanding -O2 -Wall -Wextra -Werror -fstack-protector-all
    $<$<LANG:C>:-std=gnu99>
    $<$<LANG:CXX>:-fno-exceptions -fno-rtti>)

if (NOT DEFINED LIBC_TARGET)
    set(LIBC_TARGET lambos-libc)
    message (WARNING "LIBC_TARGET was not set by project root. Defaulting to '${LIBC_TARGET}'.")
    set(LIBC_TARGET ${LIBC_TARGET} PARENT_SCOPE)
endif ()
if (NOT DEFINED LIBK_TARGET)
    set(LIBK_TARGET lambos-libk)
    message (WARNING "LIBK_TARGET was not set by project root. Defaulting to '${LIBK_TARGET}'.")
    set(LIBK_TARGET ${LIBK_TARGET} PARENT_SCOPE)
endif ()

set (STDIO_SOURCES
        src/stdio/gets.c
        src/stdio/printf.c
        )

set (STDLIB_SOURCES
        src/stdlib/exit.c
        src/stdlib/liballoc.c
        src/stdlib/itoa.c
        src/stdlib/liballoc_hook.cpp
        )

set (STRING_SOURCES
        src/string/memset.cpp
        src/string/string.c
        src/string/strtok.c
        )

set (SOURCES
    ${STDIO_SOURCES}
    ${STDLIB_SOURCES}
    ${STRING_SOURCES}
    src/ctype.c
    src/sys/syscall.cpp
    src/sys/syscall_kernel.cpp
    src/lang_support.cpp)

macro(add_libc TARGET_NAME IS_KERNEL)
    add_library(${TARGET_NAME} STATIC ${SOURCES})
    lambos_add_compile_options(${TARGET_NAME})
    target_link_libraries(${TARGET_NAME} gcc)
    target_include_directories(${TARGET_NAME} PUBLIC include)
    target_include_directories(${TARGET_NAME} PRIVATE src syscall/include)
    target_compile_features(${TARGET_NAME} PUBLIC ${LAMBOS_CXX_STANDARD_FEATURE})

    set(LIBC_COMMON_WARNING_FLAGS -Wno-unused-function)
    target_compile_options(${TARGET_NAME} PRIVATE
            "$<$<COMPILE_LANGUAGE:C,CXX>:${LIBC_COMMON_WARNING_FLAGS}>")

    if (${IS_KERNEL})
        message("Generating libc target '${TARGET_NAME}' for kernel target")
        target_compile_options(${TARGET_NAME} PRIVATE "$<$<COMPILE_LANGUAGE:C,CXX>:-DKERNEL>")
        target_include_directories(${TARGET_NAME} PRIVATE $<TARGET_PROPERTY:${KERNEL_TARGET},INTERFACE_INCLUDE_DIRECTORIES>)
    else()
        message("Generating libc target '${TARGET_NAME}' for non-kernel targets")
    endif()
endmacro()

add_libc(${LIBC_TARGET} NO)
add_libc(${LIBK_TARGET} YES)

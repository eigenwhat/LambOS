if (NOT DEFINED LIBCPP_TARGET)
    set(LIBCPP_TARGET lambos-libcpp)
    message (WARNING "LIBCPP_TARGET was not set by project root. Defaulting to '${LIBCPP_TARGET}'.")
    set(LIBCPP_TARGET ${LIBCPP_TARGET} PARENT_SCOPE)
endif ()
if (NOT DEFINED LIBKPP_TARGET)
    set(LIBKPP_TARGET lambos-libkpp)
    message (WARNING "LIBKPP_TARGET was not set by project root. Defaulting to '${LIBKPP_TARGET}'.")
    set(LIBKPP_TARGET ${LIBKPP_TARGET} PARENT_SCOPE)
endif ()

set (INCLUDE_FILES
        include/cstring include/cstdio)
set (SOURCES src/new.cpp)

add_library(lambos-cxxabi OBJECT cxxabi/cpplang.cpp cxxabi/icxxabi.cpp)

macro(add_libcpp TARGET_NAME IS_KERNEL)
    add_library(${TARGET_NAME} STATIC ${SOURCES} ${INCLUDE_FILES})
    target_include_directories(${TARGET_NAME} PUBLIC include)
    target_include_directories(${TARGET_NAME} PRIVATE $<TARGET_PROPERTY:${KERNEL_TARGET},INTERFACE_INCLUDE_DIRECTORIES>)
    set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD ${LAMBOS_CXX_STANDARD})
    set(LIBCPP_CCXX_FLAGS)
    target_compile_options(${TARGET_NAME} PRIVATE
    $<$<COMPILE_LANGUAGE:C>:${C_COMPILER_FLAGS} ${LIBCPP_CCXX_FLAGS}>
    $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILER_FLAGS} ${LIBCPP_CCXX_FLAGS}>)
    target_link_libraries(${TARGET_NAME} PUBLIC lambos-cxxabi)

    if (${IS_KERNEL})
        message("Generating libcpp target '${TARGET_NAME}' for kernel target")
        set(LIBC_C_COMPILER_FLAGS ${LIBC_C_COMPILER_FLAGS} -DKERNEL)
        set(LIBC_CXX_COMPILER_FLAGS ${LIBC_CXX_COMPILER_FLAGS} -DKERNEL)
        target_include_directories(${TARGET_NAME} PRIVATE $<TARGET_PROPERTY:${KERNEL_TARGET},INTERFACE_INCLUDE_DIRECTORIES>)
        target_link_libraries(${TARGET_NAME} PUBLIC ${LIBK_TARGET})
    else()
        message("Generating libcpp target '${TARGET_NAME}' for non-kernel targets")
        target_link_libraries(${TARGET_NAME} PUBLIC ${LIBC_TARGET})
    endif()

    target_compile_options(${TARGET_NAME} PRIVATE
            $<$<COMPILE_LANGUAGE:C>:${LIBC_C_COMPILER_FLAGS}>
            $<$<COMPILE_LANGUAGE:CXX>:${LIBC_CXX_COMPILER_FLAGS}>)
endmacro()

add_libcpp(${LIBCPP_TARGET} NO)
add_libcpp(${LIBKPP_TARGET} YES)

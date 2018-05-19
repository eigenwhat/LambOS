function(add_lambos_executable)
    set(options)
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(ALEX "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_executable(${ALEX_NAME}
            ${CMAKE_SOURCE_DIR}/programs/crti.s
            ${ALEX_SOURCES}
            ${CMAKE_SOURCE_DIR}/programs/crtn.s
            ${CMAKE_SOURCE_DIR}/programs/start.c
            ${CMAKE_SOURCE_DIR}/programs/lang_support.cpp)

    target_compile_options(${ALEX_NAME} PRIVATE
            $<$<COMPILE_LANGUAGE:C>:${C_COMPILER_FLAGS}>
            $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILER_FLAGS}>)

    set_property(TARGET ${ALEX_NAME} PROPERTY LINK_FLAGS
            " -T ${CMAKE_SOURCE_DIR}/programs/link.ld -ffreestanding -O2 -nostdlib -lgcc")
endfunction()

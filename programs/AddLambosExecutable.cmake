function(add_lambos_executable)
    set(options)
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(ALEX "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_executable(${ALEX_NAME} ${ALEX_SOURCES} ${CMAKE_SOURCE_DIR}/programs/start.c)

    lambos_add_compile_options(${ALEX_NAME})
    target_compile_features(${ALEX_NAME} PUBLIC ${LAMBOS_CXX_STANDARD_FEATURE})
    set_property(TARGET ${ALEX_NAME} PROPERTY LINK_FLAGS
            " -T ${CMAKE_SOURCE_DIR}/programs/link.ld -ffreestanding -O2 -nostdlib -lgcc")
endfunction()

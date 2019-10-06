set(ELFTEST_TARGET elf-test)

set (CMAKE_EXE_LINKER_FLAGS " -T ${CMAKE_CURRENT_SOURCE_DIR}/link.ld -ffreestanding -O2 -nostdlib -lgcc")

add_executable(${ELFTEST_TARGET} main.cpp start.s lang_support.cpp)
target_compile_options(${ELFTEST_TARGET} PRIVATE
        $<$<COMPILE_LANGUAGE:C>:${C_COMPILER_FLAGS}> $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILER_FLAGS}>)
target_link_libraries(${ELFTEST_TARGET} PUBLIC ${LIBC_TARGET})

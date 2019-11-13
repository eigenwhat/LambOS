# Toolchain for i686-elf cross compiler

set (CMAKE_SYSTEM_NAME Generic)

set (CMAKE_C_COMPILER i686-elf-gcc)
set (CMAKE_CXX_COMPILER i686-elf-g++)
set (CMAKE_ASM-ATT_COMPILER i686-elf-as)

# GCC was built as follows:
#
# ../configure --target=i686-elf --prefix="<prefix>" --with-gmp=/usr/local --with-mpfr=/usr/local  --with-mpc=/usr/local
#              --disable-nls --enable-languages=c,c++ --without-headers
#
# Thus, it will probably fail to compile the test apps since it doesn't include headers or libs or anything like that.
# Therefore, force cmake to skip the test.
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

set(CRTI_PATH "${CMAKE_SOURCE_DIR}/runtime/i686-elf/crti.o")
set(CRTN_PATH "${CMAKE_SOURCE_DIR}/runtime/i686-elf/crtn.o")
execute_process(COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=crtbegin.o OUTPUT_VARIABLE CRTBEGIN_PATH)
execute_process(COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=crtend.o OUTPUT_VARIABLE CRTEND_PATH)
string(REGEX REPLACE "\n$" "" CRTBEGIN_PATH "${CRTBEGIN_PATH}")
string(REGEX REPLACE "\n$" "" CRTEND_PATH "${CRTEND_PATH}")
set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> ${CRTI_PATH} ${CRTBEGIN_PATH} <OBJECTS> ${CRTEND_PATH} ${CRTN_PATH} -o <TARGET> <LINK_LIBRARIES>")
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> ${CRTI_PATH} ${CRTBEGIN_PATH} <OBJECTS> ${CRTEND_PATH} ${CRTN_PATH} -o <TARGET> <LINK_LIBRARIES>")

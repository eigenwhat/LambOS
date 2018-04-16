# Toolchain for i586-elf cross compiler

set (CMAKE_SYSTEM_NAME Generic)

set (CMAKE_C_COMPILER i586-elf-gcc)
set (CMAKE_CXX_COMPILER i586-elf-g++)
set (CMAKE_ASM_ATT_COMPILER i586-elf-as)

# GCC was built as follows:
#
# ../configure --target=i586-elf --prefix="<prefix>" --with-gmp=/usr/local --with-mpfr=/usr/local  --with-mpc=/usr/local
#              --disable-nls --enable-languages=c,c++ --without-headers
#
# Thus, it will probably fail to compile the test apps since it doesn't include headers or libs or anything like that.
# Therefore, force cmake to skip the test.
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

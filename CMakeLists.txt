cmake_minimum_required(VERSION 3.5)
project(lambos VERSION 0.1 LANGUAGES C CXX ASM-ATT)

set (COMMON_COMPILER_FLAGS -ffreestanding -O2 -Wall -Wextra -Werror -fstack-protector-all)
set (C_COMPILER_FLAGS ${COMMON_COMPILER_FLAGS} -std=gnu99)
set (CXX_COMPILER_FLAGS ${COMMON_COMPILER_FLAGS} -fno-exceptions -fno-rtti)

set (CMAKE_EXE_LINKER_FLAGS " -T ${CMAKE_SOURCE_DIR}/linker.ld -ffreestanding -O2 -nostdlib -lgcc")

# libc target name
set (LIBC_TARGET lambosc)
# libc++ target name
set (LIBCPP_TARGET lambosc++)
# lambos system library target name
set (LIBSYS_TARGET lambos)

# define kernel headers; used by several subprojects
set(KERNEL_HEADERS_TARGET lambos-kernel-headers)
add_library(${KERNEL_HEADERS_TARGET} INTERFACE)
target_include_directories(${KERNEL_HEADERS_TARGET} INTERFACE kernel/include libsys/include)

add_subdirectory(libc)
add_subdirectory(libcpp)
add_subdirectory(libsys)
add_subdirectory(kernel)

# create bootable ISO
find_program(GRUB_MKRESCUE grub-mkrescue)
if (NOT GRUB_MKRESCUE)
    message (WARNING "Unable to find `grub-mkrescue`. Bootable ISO image generation will not be available.")
else ()
    #$(OBJDIR)/lambos.iso: $(OBJDIR)/kernel.bin
    #@cp -r iso $(OBJDIR)/iso
    #@cp $(OBJDIR)/kernel.bin $(OBJDIR)/iso/boot/
    #grub-mkrescue --output=$(OBJDIR)/lambos.iso $(OBJDIR)/iso
    file (COPY ${CMAKE_SOURCE_DIR}/iso DESTINATION ${CMAKE_BINARY_DIR})
    set (LAMBOS_ISO_TARGET grub-bootable-iso)
    set (LAMBOS_ISO_PATH ${CMAKE_BINARY_DIR}/lambos.iso)
    add_custom_target(${LAMBOS_ISO_TARGET}
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${KERNEL_TARGET}> ${CMAKE_BINARY_DIR}/iso/boot/
            COMMAND ${GRUB_MKRESCUE} --output=${LAMBOS_ISO_PATH} ${CMAKE_BINARY_DIR}/iso)
    add_dependencies(${LAMBOS_ISO_TARGET} ${KERNEL_TARGET})

    # set up convenience target to boot generated ISO in qemu (if available)
    find_program(QEMU qemu-system-i386)
    if (QEMU)
        set (RUN_QEMU_TARGET run-qemu)
        add_custom_target(${RUN_QEMU_TARGET} COMMAND ${QEMU} -cdrom ${LAMBOS_ISO_PATH} -m 32)
    endif ()

    # set up convenience target to boot generated ISO in bochs (if available)
    find_program(BOCHS bochs)
    if (BOCHS)
        # build bochsrc to run off of
        set(BOCHS_BIOS_PATH /usr/share/bochs)
        configure_file(${CMAKE_SOURCE_DIR}/bochsrc.in ${CMAKE_BINARY_DIR}/.bochsrc)

        # set up custom run target
        set (RUN_BOCHS_TARGET run-bochs)
        add_custom_target(${RUN_BOCHS_TARGET} COMMAND ${BOCHS} -q WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    endif ()
endif ()
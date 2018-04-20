cmake_minimum_required(VERSION 3.8)
project(lambos VERSION 0.1 LANGUAGES C CXX ASM-ATT)

set (COMMON_COMPILER_FLAGS -ffreestanding -O2 -Wall -Wextra -Werror -fstack-protector-all)
set (C_COMPILER_FLAGS ${COMMON_COMPILER_FLAGS} -std=gnu99)
set (CXX_COMPILER_FLAGS ${COMMON_COMPILER_FLAGS} -fno-exceptions -fno-rtti)

set (CMAKE_EXE_LINKER_FLAGS " -T ${CMAKE_SOURCE_DIR}/linker.ld -ffreestanding -O2 -nostdlib -lgcc")

# options

# By default, bochs will just figure out on its own what display library to use.
option(BOCHS_USE_X "Tell bochs to use X display library" OFF)
option(BOCHS_USE_SDL "Tell bochs to use the SDL display library" OFF)
option(BOCHS_USE_SDL2 "Tell bochs to use the SDL2 display library" OFF)
option(BOCHS_USE_WIN32 "Tell bochs to use the Win32 display library" OFF)
option(BOCHS_USE_GUI_DEBUG "Tell bochs to use the GUI debugger if able. Requires specifying a display library." OFF)

# Define all target names here, as they're somewhat interdependent.
# kernel target name
set(KERNEL_TARGET lambos-kernel)
# libc target name
set (LIBC_TARGET lambosc)
# libc++ target name
set (LIBCPP_TARGET lambosc++)
# lambos system library target name
set (LIBSYS_TARGET lambos)

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
    set (ISO_TARGET grub-bootable-iso)
    set (ISO_PATH ${CMAKE_BINARY_DIR}/lambos.iso)
    add_custom_target(${ISO_TARGET}
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${KERNEL_TARGET}> ${CMAKE_BINARY_DIR}/iso/boot/
            COMMAND ${GRUB_MKRESCUE} --output=${ISO_PATH} ${CMAKE_BINARY_DIR}/iso)
    add_dependencies(${ISO_TARGET} ${KERNEL_TARGET})

    # set up convenience target to boot generated ISO in qemu (if available)
    find_program(QEMU qemu-system-i386)
    if (QEMU)
        set (RUN_QEMU_TARGET run-qemu)
        add_custom_target(${RUN_QEMU_TARGET} COMMAND ${QEMU} -cdrom ${ISO_PATH} -m 32)
        add_dependencies(${RUN_QEMU_TARGET} ${ISO_TARGET})
    endif ()

    # set up convenience target to boot generated ISO in bochs (if available)
    find_program(BOCHS bochs)
    if (BOCHS)
        # build bochsrc to run off of
        set(BOCHS_BIOS_PATH "\$BXSHARE")

        if (BOCHS_USE_X)
            set(BOCHS_DISPLAY_LIBRARY x)
        elseif (BOCHS_USE_SDL)
            set(BOCHS_DISPLAY_LIBRARY sdl)
        elseif (BOCHS_USE_SDL2)
            set(BOCHS_DISPLAY_LIBRARY sdl2)
        elseif (BOCHS_USE_WIN32)
            set(BOCHS_DISPLAY_LIBRARY win32)
        endif ()

        if (BOCHS_DISPLAY_LIBRARY)
            if (BOCHS_USE_GUI_DEBUG)
                set(BOCHS_DISPLAY_OPTIONS "display_library: ${BOCHS_DISPLAY_LIBRARY}, options=\"gui_debug\"")
            else ()
                set(BOCHS_DISPLAY_OPTIONS "display_library: ${BOCHS_DISPLAY_LIBRARY}")
            endif ()
        endif ()

        set (BOCHS_ATA0_PATH ${ISO_PATH})
        configure_file(${CMAKE_SOURCE_DIR}/bochsrc.in ${CMAKE_BINARY_DIR}/.bochsrc)

        # set up custom run target
        set (RUN_BOCHS_TARGET run-bochs)
        add_custom_target(${RUN_BOCHS_TARGET} COMMAND ${BOCHS} -q WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
        add_dependencies(${RUN_BOCHS_TARGET} ${ISO_TARGET})
    endif ()
endif ()
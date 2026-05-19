Lamb Operating System
========

Overview
--------
LambOS is a hobby operating system with three intentions:
* Fulfill a personal long-standing goal to write an operating system.
* Gain experience in low-level software engineering and learn more about
  computer architecture.
* Make a codebase that is easy for OS dev beginners to read and digest
  in small chunks (i.e. *NOT* your typical terse, uncommented C code).

Name
----
An allegory to the humble OS developer, naive and optimistic before
hitting the confidence buzzsaw that is bare metal programming.

That, or it's a play off my old boss's NetID which had evolved into some
sort of inside joke, the kind nobody outside that office could ever
truly understand and only really ever serves to annoy other people. But,
it worked out neatly enough for the name of an operating system.

I think I'm going to rename it after guinea pigs at some point, because
you know, experimenting with OS development, being a guinea pig...

Also, have you SEEN THOSE THINGS??

<img src="https://i.redd.it/qnbtas3gw1n01.jpg"
     alt="The cutest little thing ever"
     title="It's a fuzzy potato with the most adorable face!"
     width=800 height=600 />

How to build
------------

In order to build LambOS, you will need a cross-compiler set up.
Instructions can be found [here](https://wiki.osdev.org/GCC_Cross-Compiler).

The project is also configured to generate a bootable ISO. While not
strictly necessary, it's useful and convenient. It is built using the
grub-mkrescue utility, which requires the following:

* GRUB 2.0
* xorriso

Finally, you'll need something to run this with. Included in the project
are convenience targets to boot the generated image with either Bochs or
QEMU (specifically, QEMU for the i386 system target).

### CMake

In order to build with CMake, you'll need to tell it where to find your
cross compiler. Included in the cmake/ folder are two simple toolchain
definitions for i586-elf and i686-elf. To use them, set the 
`CMAKE_TOOLCHAIN_FILE` variable to the file path of the toolchain file
as part of the CMake invocation.

The following targets are available:

* `lambos-kernel`: Builds the kernel.
* `grub-bootable-iso`: Builds a bootable ISO. Only available if CMake
    is able to locate grub-mkrescue on your machine.
* `run-bochs`: Configures and launches Bochs with the bootable ISO
    loaded. Only available if Bochs is installed (and CMake found it).
    By default it figures out on its own which display library to use.
    This and more can be configured by enabling the corresponding CMake
    options.
* `run-qemu`: Configures and launches QEMU with the bootable ISO loaded.
    Only available if QEMU (specifically qemu-system-i386) is installed.

The following CMake options are available for setting:

* `BOCHS_USE_X`: Tells bochs to use the X11 display library. (OFF)
* `BOCHS_USE_SDL`: Tells bochs to use the SDL display library. (OFF)
* `BOCHS_USE_SDL2`: Tells bochs to use the SDL2 display library. (OFF)
* `BOCHS_USE_WIN32`: Tells bochs to use the Win32 display library." (OFF)
* `BOCHS_USE_GUI_DEBUG`: Tells bochs to use the GUI debugger if able.
    Requires explicitly specifying a display library." (OFF)

* `QEMU_USE_GDB`: Tells QEMU to run with the gdbserver enabled and wait
    for a signal from the connection before booting. (OFF)

Page fault analysis helper
--------------------------

A helper script is available at `scripts/page_fault_report.py` to analyze
the panic string emitted by `PageFaultISR` and run symbolization utilities
(`addr2line`, `nm`, `readelf`, `objdump`) against the built kernel binary.

Example:

```bash
./scripts/page_fault_report.py "Page Fault: cr2=0xFFC00004 eip=0xC0112498/in Kernel mode/"
```

JSON output (useful for automation/coding agents):

```bash
./scripts/page_fault_report.py --format json "Page Fault: cr2=0xFFC00004 eip=0xC0112498/in Kernel mode/"
```

The script auto-detects:

* Kernel image path (prefers `build/kernel/kernel.bin`, then `cmake-build-debug/kernel/kernel.bin`)
* Binutils tools from CMake cache (`CMAKE_ADDR2LINE`, `CMAKE_OBJDUMP`, etc.)

Useful options:

* `--kernel <path>`: Explicit kernel binary/ELF path
* `--build-dir <path>`: Build directory to inspect (repeatable)
* `--tool-prefix i686-elf-`: Force a specific cross-tool prefix
* `--objdump-window 0xA0`: Increase disassembly context around EIP
* `--output report.txt`: Save report to a file
* `--format json`: Emit a machine-readable report

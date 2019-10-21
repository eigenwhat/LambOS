//
// Created by Martin Miralles-Cordal on 10/21/2019.
//

int main(int argc, char *argv[]);

// provided by crti.o. Though, until we either build the OS-specific toolchain
// or figure out how to direct CMake to link crtbegin/crtend.o in the correct
// order, this is really just theater as the functions do nothing.
void _init();
void _fini();
void __cxa_finalize();

// should really be void and use exit(), but we don't have proper process
// support yet, so that's just going to have to wait
int _start(int argc, char *argv[])
{
    _init();
    int ret = main(argc, argv);
    __cxa_finalize(0);
    _fini();
    return ret;
}

//
// Created by Martin Miralles-Cordal on 10/22/19.
//

#include <sys/syscall.h>

void exit(int exit_code) { sys_exit(exit_code); }

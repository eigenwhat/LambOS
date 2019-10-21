//
// Created by Martin Miralles-Cordal on 5/14/2018.
//

#include <stdio.h>

struct Foo
{
    struct Static
    {
        Static() { puts("[Static::Static()] Hello, world!"); }
        ~Static() { puts("[Static::~Static()] Goodbye, cruel world!"); }
    } ayy;
};

[[maybe_unused]] static Foo foo{};

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
    puts("Hello, system call!");
    return (int)0xCAFEBABE;
}

//
// Created by Martin Miralles-Cordal on 9/1/2013.
//

#include <io/PrintStream.hpp>
#include <cstdlib>

namespace _ns_LIBSYS {

void PrintStream::print(char const *str)
{
    char currLetter;
    while((currLetter = *str++)) print(currLetter);
}

} // libsys namespace

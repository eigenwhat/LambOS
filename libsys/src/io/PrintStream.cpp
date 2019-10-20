#include <io/PrintStream.hpp>
#include <stdlib.h>

void PrintStream::print(char const *str)
{
    char currLetter;
    while((currLetter = *str++)) print(currLetter);
}
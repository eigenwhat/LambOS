#pragma once
#include <io/FilterOutputStream.hpp>

class PrintStream : public FilterOutputStream
{
public:
    PrintStream(OutputStream &out) : FilterOutputStream(out) {}
    virtual void print(char character) { _out.write((uint8_t)character); }
    virtual void print(const char *str);
    virtual void print(int num);
    virtual void println() { print('\n'); flush(); }
    template<typename T> void println(T item) { print(item); println(); }
};

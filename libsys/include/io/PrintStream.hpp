#pragma once

#include <io/FilterOutputStream.hpp>
#include <concepts>
#include <cstdlib>

class PrintStream : public FilterOutputStream
{
    template <std::integral T>
    static void xtoa(T num, char *intstr, int base);
  public:
    PrintStream(OutputStream &out) : FilterOutputStream(out) {}

    void print(char character) { _out.write((std::byte)character); }
    void print(char const *str);

    template <std::integral Integral>
    void print(Integral item)
    {
        char intstr[33];
        xtoa(item, intstr, 10);
        print(intstr);
    }

    void println() { print('\n'); flush(); }
    template<typename T> void println(T item) { print(item); println(); }
};

template <> inline void PrintStream::xtoa(int num, char *intstr, int base)                { itoa(num, intstr, base); }
template <> inline void PrintStream::xtoa(long int num, char *intstr, int base)           { ltoa(num, intstr, base); }
template <> inline void PrintStream::xtoa(long long int num, char *intstr, int base)      { lltoa(num, intstr, base); }
template <> inline void PrintStream::xtoa(unsigned int num, char *intstr, int base)       { uitoa(num, intstr, base); }
template <> inline void PrintStream::xtoa(unsigned long num, char *intstr, int base)      { ultoa(num, intstr, base); }
template <> inline void PrintStream::xtoa(unsigned long long num, char *intstr, int base) { ulltoa(num, intstr, base); }

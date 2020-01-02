#pragma once

#include <io/FilterOutputStream.hpp>
#include <io/Print.hpp>

namespace sys {

class PrintStream : public FilterOutputStream
{
  public:
    PrintStream(OutputStream &out) : FilterOutputStream(out) {}

    template <typename T>
    void print(T &&item)
    {
        sys::print(*this, std::forward<T>(item));
    }

    void println() { print('\n'); flush(); }
    template<typename T> void println(T item) { print(item); println(); }
};

} // libsys namespace

//
// Created by Martin Miralles-Cordal on 1/27/2014.
//

#pragma once

#include <Memory.hpp>
#include <device/display/Console.hpp>
#include <io/InputStream.hpp>
#include <io/OutputStream.hpp>

class Context
{
  public:
    Context() : _console(nullptr), _out(nullptr) { }

    /**
     * Gets the Console for this Context.
     * @return The Console.
     */
    Console* console() const { return _console; }

    /**
     * Sets the Console for this Context.
     * @param console The Console to set.
     */
    void setConsole(Console *console) { _console = console; }

    /**
     * Returns stdout for this Context.
     * @return An OutputStream connected to stdout.
     */
    sys::OutputStream *out() const { return _out; }

    /**
     * Sets the OutputStream for the stdout of this Context.
     * @param out The OutputStream to set.
     */
    void setOut(sys::OutputStream *out) { _out = out; }

    /**
     * Returns stdin for this Context.
     * @return An InputStream connected to stdin.
     */
    sys::InputStream *in() const { return _in; }

    /**
     * Sets the InputStream for the stdin of this Context.
     * @param in The InputStream to set.
     */
    void setIn(sys::ArcPtr<sys::InputStream> in) { _in = std::move(in); }

  private:
    Console *_console;
    sys::OutputStream *_out;
    sys::ArcPtr<sys::InputStream> _in;
};

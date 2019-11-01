//
// Created by Martin Miralles-Cordal on 1/27/2014.
//

#pragma once

#include <Object.hpp>
#include <device/display/Console.hpp>
#include <io/PrintStream.hpp>
#include <io/InputStream.hpp>

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
     * @return A PrintStream connected to stdout.
     */
    sys::PrintStream *out() const { return _out.get(); }

    /**
     * Sets the PrintStream for the stdout of this Context.
     * @param out The PrintStream to set.
     */
    void setOut(sys::PrintStream *out) { _out = out; }

    /**
     * Returns stdin for this Context.
     * @return An InputStream connected to stdin.
     */
    sys::InputStream *in() const { return _in.get(); }

    /**
     * Sets the InputStream for the stdin of this Context.
     * @param in The InputStream to set.
     */
    void setIn(sys::ArcPtr<sys::InputStream> in) { _in = std::move(in); }

  private:
    Console *_console;
    sys::ArcPtr<sys::PrintStream> _out;
    sys::ArcPtr<sys::InputStream> _in;
};

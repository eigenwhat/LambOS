#include <Object.hpp>
#include <device/display/Console.hpp>
#include <io/PrintStream.hpp>

class Context : virtual public Object
{
public:
    Context() : _console(NULL), _out(NULL) { }
    void setConsole(Console *console) { this->_console = console; }
    Console* console() { return this->_console; }
    void setOut(PrintStream *out) { this->_out = out; }
    PrintStream *out() { return this->_out; }
private:
    Console *_console;
    PrintStream *_out;
};

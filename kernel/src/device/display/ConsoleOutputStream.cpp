#include <device/display/ConsoleOutputStream.hpp>

namespace {

void doBackspace(Console &console) {
    if (console.column() == 0) {
        if (console.row() == 0) {
            console.putCharAt(' ', 0, 0);
            return;
        } else {
            console.moveTo(console.row() - 1, console.width() - 1);
        }
    } else {
        console.moveTo(console.row(), console.column() - 1);
    }

    console.putCharAt(' ', console.row(), console.column());
}

}

void ConsoleOutputStream::write(std::byte byte)
{
    char c = (char)byte;
    switch (c) {
        case '\b':
            doBackspace(_console); break;
        default:
            _console.putChar(c);
    }
}

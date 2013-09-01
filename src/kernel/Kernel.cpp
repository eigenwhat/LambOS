#include <string.h>
#include "Kernel.hpp"

//======================================================
// Kernel
//======================================================
void Kernel::panic(const char *string)
{
    size_t length = strlen(string);
    terminal()->setBackgroundColor(COLOR_RED);
    terminal()->setForegroundColor(COLOR_WHITE);
    terminal()->clear();
    const char *banner[3]= {"* * * * * * * * * * * * * * * * * * * *",
                            "* LAMBOS FOUND OUT ABOUT WHAT YOU DID *",
                            "* * * * * * * * * * * * * * * * * * * *"};
    for(int i = 0; i < 3; ++i) {
        size_t bannerlength = strlen(banner[i]);
        size_t columnToCenter = terminal()->width() > bannerlength ? (terminal()->width() - bannerlength) / 2 : 0;
        terminal()->moveTo(i+1,columnToCenter);
        terminal()->writeString(banner[i]);
    }
    
    size_t rowToCenter = terminal()->height()/2;
    size_t columnToCenter = (terminal()->width() - length) / 2;
    terminal()->moveTo(rowToCenter, columnToCenter);
    terminal()->writeString(string);

    const char *msg = "0xL4MBOS";
    terminal()->moveTo(terminal()->height()-2, (terminal()->width() - strlen(msg)) / 2);
    terminal()->writeString(msg);

    asm("cli\nhlt");
}

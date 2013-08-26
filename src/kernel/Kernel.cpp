#include <string.h>
#include "Kernel.hpp"

//======================================================
// Kernel
//======================================================
void Kernel::panic(const char *string)
{
    size_t length = strlen(string);
    out()->setBackgroundColor(COLOR_RED);
    out()->setForegroundColor(COLOR_WHITE);
    out()->clear();
    const char *banner[3]= {"* * * * * * * * * * * * * * * * * * * *",
                            "* LAMBOS FOUND OUT ABOUT WHAT YOU DID *",
                            "* * * * * * * * * * * * * * * * * * * *"};
    for(int i = 0; i < 3; ++i) {
        size_t bannerlength = strlen(banner[i]);
        size_t columnToCenter = out()->width() > bannerlength ? (out()->width() - bannerlength) / 2 : 0;
        out()->moveTo(i+1,columnToCenter);
        out()->writeString(banner[i]);
    }
    
    size_t rowToCenter = out()->height()/2;
    size_t columnToCenter = (out()->width() - length) / 2;
    out()->moveTo(rowToCenter, columnToCenter);
    out()->writeString(string);

    const char *msg = "0xL4MBOS";
    out()->moveTo(out()->height()-2, (out()->width() - strlen(msg)) / 2);
    out()->writeString(msg);
}

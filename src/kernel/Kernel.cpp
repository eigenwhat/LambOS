#include <string.h>
#include "Kernel.hpp"

//======================================================
// Kernel
//======================================================
void Kernel::panic(const char *string)
{
    size_t length = strlen(string);
    stdout()->setBackgroundColor(COLOR_RED);
    stdout()->setForegroundColor(COLOR_WHITE);
    stdout()->clear();
    const char *banner[3]= {"* * * * * * * * * * * * * * * * * * * *",
                            "* LAMBOS FOUND OUT ABOUT WHAT YOU DID *",
                            "* * * * * * * * * * * * * * * * * * * *"};
    for(int i = 0; i < 3; ++i) {
        size_t bannerlength = strlen(banner[i]);
        size_t columnToCenter = stdout()->width() > bannerlength ? (stdout()->width() - bannerlength) / 2 : 0;
        stdout()->moveTo(i+1,columnToCenter);
        stdout()->writeString(banner[i]);
    }
    
    size_t rowToCenter = stdout()->height()/2;
    size_t columnToCenter = (stdout()->width() - length) / 2;
    stdout()->moveTo(rowToCenter, columnToCenter);
    stdout()->writeString(string);

    const char *msg = "0xL4MBOS";
    stdout()->moveTo(stdout()->height()-2, (stdout()->width() - strlen(msg)) / 2);
    stdout()->writeString(msg);
}

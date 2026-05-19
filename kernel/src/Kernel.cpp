//
// Created by Martin Miralles-Cordal on 8/8/2013.
//

#include <Kernel.hpp>
#include <system/Debug.hpp>
#include <util/StringView.hpp>

#include <cstring>

//======================================================
// Kernel
//======================================================
void Kernel::panic(char const *string)
{
    DEBUG_BREAK();
    size_t length = strlen(string);
    console()->setBackgroundColor(COLOR_RED);
    console()->setForegroundColor(COLOR_WHITE);
    console()->clear();
    char const *banner[3] = {"* * * * * * * * * * * * * * * * * * * *",
                             "* LAMBOS FOUND OUT ABOUT WHAT YOU DID *",
                             "* * * * * * * * * * * * * * * * * * * *"};
    for (size_t i = 0; i < 3; ++i) {
        size_t bannerlength = strlen(banner[i]);
        size_t columnToCenter = console()->width() > bannerlength ? (console()->width() - bannerlength) / 2 : 0;
        console()->moveTo(i + 1, columnToCenter);
        console()->writeString(banner[i]);
    }

    size_t rowToCenter = console()->height() / 2;
    const auto maxLineWidth = console()->width() - 4; // Leave some padding on the sides
    if (length > maxLineWidth)
    {
        sys::StringView wholeStr{string};
        auto estimatedLines = length / maxLineWidth + static_cast<std::size_t>(length % console()->width() != 0);
        std::size_t row = rowToCenter - (estimatedLines / 2);
        for (std::size_t i = 0; i < estimatedLines; ++i) {
            sys::StringView line = wholeStr.substring(0, maxLineWidth);
            if (line.isEmpty()) {
                break;
            }
            for (std::size_t j = line.size(); j > 0; --j) {
                if (line[j-1] == ' ') {
                    line = line.substring(0, j);
                    break;
                }
            }
            size_t columnToCenter = (console()->width() - line.size()) / 2;
            console()->moveTo(row++, columnToCenter);
            console()->writeString(line.data(), line.size());
            wholeStr = wholeStr.substring(line.size());
        }
    }
    else {
        size_t columnToCenter = (console()->width() - length) / 2;
        console()->moveTo(rowToCenter, columnToCenter);
        console()->writeString(string);
    }

    char const *msg = "0xL4MBOS";
    console()->moveTo(console()->height() - 2, (console()->width() - strlen(msg)) / 2);
    console()->writeString(msg);

    sys::debug_println("Kernel panic: %@", string);
    asm("cli\nhlt");
}

#include <cstdio>
#include <cstring>
#include <ctype.h>
#include <sys/syscall.h>

#include <algorithm>
#include <util/String.hpp>

enum class Command {
    kExit,
    kWho,
    kEcho,
    kSleep,
    kDie,
    kUnknown
};

void who()
{
    printf("Who are you? ");
    char str[100];
    gets(str);
    printf("Hello, %s!\n", str);
}

void unknown()
{
    puts("Unrecognized command. Try harder.");
}

void echo(char const *str)
{
    puts(str);
}

int pow(int base, unsigned exp)
{
    int result = 1;
    for (unsigned i = 1; i <= exp; i++)
    {
        result *= base;
    }

    return result;
}

void sleep(char const *str)
{
    int secs = 0;
    unsigned place = 0;
    sys::String timeString{str};
    for (auto it = std::rbegin(timeString); it != std::rend(timeString); it++)
    {
        if (!isdigit(*it))
        {
            return;
        }

        int digit = *it - '0';
        secs += digit * pow(10, place);
        ++place;
    }

    printf("Sleeping for %d seconds\n", secs);

    sys_sleep(secs);
}

Command parseCommand(char const *token)
{
    Command cmd = Command::kUnknown;
    if (!strcmp(token, "who")) {
        cmd = Command::kWho;
    } else if (!strcmp(token, "exit") || !strcmp(token, "quit")) {
        cmd = Command::kExit;
    } else if (!strcmp(token, "echo")) {
        cmd = Command::kEcho;
    } else if (!strcmp(token, "sleep")) {
        cmd = Command::kSleep;
    } else if (!strcmp(token, "die") || !strcmp(token, "sudoku")) {
        cmd = Command::kDie;
    }

    return cmd;
}

Command readCommand()
{
    char str[100];
    gets(str);
    auto len = strlen(str);

    char const *arg0 = strtok(str, " ");
    auto toklen = strlen(arg0);

    char const *argv = toklen < len ? str+toklen+1 : str+toklen;

    const Command cmd = parseCommand(arg0);
    switch (cmd) {
        case Command::kWho:
            who(); break;
        case Command::kUnknown:
            unknown(); break;
        case Command::kEcho:
            echo(argv); break;
        case Command::kSleep:
            sleep(argv); break;
        case Command::kDie:
            sys_die(); break;
        case Command::kExit: break;
    }

    return cmd;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
    Command c;
    do {
        printf("kvsh |#> ");
        c = readCommand();
    } while (c != Command::kExit);

    return (int)0xDEADC0DE;
}

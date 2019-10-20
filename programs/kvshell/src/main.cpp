#include <cstdio>
#include <cstring>

#include <util/String.hpp>

enum class Command {
    kExit,
    kWho,
    kEcho,
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

Command readCommand()
{
    char str[100];
    gets(str);
    auto len = strlen(str);

    char *argc = strtok(str, " ");
    auto toklen = strlen(argc);

    char *argv = toklen < len ? str+toklen+1 : str+toklen;

    Command cmd = Command::kUnknown;
    if (!strcmp(argc, "who")) {
        cmd = Command::kWho;
    } else if (!strcmp(argc, "exit") || !strcmp(argc, "quit")) {
        cmd = Command::kExit;
    } else if (!strcmp(argc, "echo")) {
        cmd = Command::kEcho;
    }

    switch (cmd) {
        case Command::kWho:
            who(); break;
        case Command::kUnknown:
            unknown(); break;
        case Command::kEcho:
            echo(argv); break;
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

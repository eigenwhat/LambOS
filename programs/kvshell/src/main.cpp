#include <cstdio>

int main(int argc, char *argv[])
{
    printf("Who are you? ");
    char str[100];
    gets(str);
    printf("Hello, %s!\n", str);
    return 0xDEADC0DE;
}

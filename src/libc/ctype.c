#include <ctype.h>

/* Check if character is alphanumeric */
int isalnum(int c)
{
    return isalpha(c) || isdigit(c);
}

/* Check if character is alphabetic */
int isalpha(int c)
{
    return islower(c) || isupper(c);
}

/* Check if character is blank */
int isblank(int c)
{
    return c == ' ' || c == '\t';
}

/* Check if character is a control character */
int iscntrl(int c)
{
    return (c >= 0 && c <= 0x1f) || c == 0x7f;
}

/* Check if character is decimal digit */
int isdigit(int c)
{
    return (c >= '0' && c <= '9');
}

/* Check if character has graphical representation */
int isgraph(int c)
{
    return isprint(c) && c != ' ';
}

/* Check if character is lowercase letter */
int islower(int c)
{
    return (c >= 'a' && c <= 'z');
}

/* Check if character is printable */
int isprint(int c)
{
    return !iscntrl(c);
}

/* Check if character is a punctuation character */
int ispunct(int c)
{
    return isgraph(c) && !isalnum(c);
}

/* Check if character is a white-space */
int isspace(int c)
{
    switch(c) {
        case ' ':
        case '\n':
        case '\t':
        case '\f':
        case '\v':
        case '\r':
            return 1;
        default:
            return 0;
    }
}

/* Check if character is uppercase letter */
int isupper(int c)
{
    return (c >= 'A' && c <= 'Z');
}

/* Check if character is hexadecimal digit */
int isxdigit(int c)
{
    return isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

int tolower(int c)
{
    if(isupper(c)) {
        return c + 0x20;
    } else {
        return c;
    }
}

int toupper(int c)
{
    if(islower(c)) {
        return c - 0x20;
    } else {
        return c;
    }
}

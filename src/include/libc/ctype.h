#ifndef CTYPE_H
#define CTYPE_H

#include <stddef.h>
#include <stdint.h>
#include <decl.h>

__BEGIN_DECLS

/* Check if character is alphanumeric */
int isalnum(int c);

/* Check if character is alphabetic */
int isalpha(int c);

/* Check if character is blank */
int isblank(int c);

/* Check if character is a control character */
int iscntrl(int c);

/* Check if character is decimal digit */
int isdigit(int c);

/* Check if character has graphical representation */
int isgraph(int c);

/* Check if character is lowercase letter */
int islower(int c);

/* Check if character is printable */
int isprint(int c);

/* Check if character is a punctuation character */
int ispunct(int c);

/* Check if character is a white-space */
int isspace(int c);

/* Check if character is uppercase letter */
int isupper(int c);

/* Check if character is hexadecimal digit */
int isxdigit(int c);

int toupper(int c);

int tolower(int c);

__END_DECLS

#endif
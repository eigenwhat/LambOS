#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>
#include <stdint.h>
#include <decl.h>

__BEGIN_DECLS

#define MIN(a, b) (a) > (b) ? (b) : (a)
#define MAX(a, b) (a) > (b) ? (a) : (b)
#define CLAMP(num, min, max) num = MAX(MIN((num), (max)), (min))

char *itoa(int value, char *str, int base);

__END_DECLS

#endif
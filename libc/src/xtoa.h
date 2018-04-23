#ifndef LAMBOS_ITOA_PRIVATE_H
#define LAMBOS_ITOA_PRIVATE_H

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <decl.h>

__BEGIN_DECLS

/**
 * Unsigned int to string.
 * @param value The unsigned to stringify.
 * @param str The char buffer to place the string.
 * @param base The numeric base to generate the number in. [2-32]
 * @return A pointer to the string for convenience. (equal to str)
 */
char *uitoa(unsigned int value, char *str, int base);

/**
 * Long to string.
 * @param value The long to stringify.
 * @param str The char buffer to place the string.
 * @param base The numeric base to generate the number in. [2-32]
 * @return A pointer to the string for convenience. (equal to str)
 */
char *ltoa(long value, char *str, int base);

/**
 * Long long to string.
 * @param value The long to stringify.
 * @param str The char buffer to place the string.
 * @param base The numeric base to generate the number in. [2-32]
 * @return A pointer to the string for convenience. (equal to str)
 */
char *lltoa(long long value, char *str, int base);

__END_DECLS

#endif //LAMBOS_ITOA_PRIVATE_H

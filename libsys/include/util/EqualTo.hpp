#ifndef LAMBOS_EQUALTO_HPP
#define LAMBOS_EQUALTO_HPP

#include <cstring>

/**
 * Function object for performing equality comparisons. Unless specialised,
 * invokes operator== on type T.
 */
template<typename T>
struct EqualTo
{
    constexpr bool operator()(const T &lhs, const T &rhs) const
    {
        return lhs == rhs;
    }
};

template<> struct EqualTo<char *>
{
    bool operator()(char const *lhs, char const *rhs) const
    {
        return lhs == rhs || !strcmp(lhs, rhs);
    }
};

template<> struct EqualTo<char const *> : public EqualTo<char *> {};

#endif //LAMBOS_EQUALTO_HPP

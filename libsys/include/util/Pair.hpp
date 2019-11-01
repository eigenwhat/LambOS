//
// Created by Martin Miralles-Cordal on 4/29/2018.
//

#pragma once

#include <Object.hpp>

namespace _ns_LIBSYS {

/** A Pair is a pairing of two objects of possibly differing types. */
template <typename A, typename B> class Pair : public virtual Object
{
  public:
    using FirstType = A;
    using SecondType = B;

    Pair() = default;

    Pair(const A &a, const B &b) : first(a), second(b) {}

    /** The first element in the pair. */
    A first;

    /** The second element in the pair. */
    B second;

    /**
     * Assigns this pair's values to the values contained in another pair.
     * @param rhs The pair to copy values from.
     * @return A reference to the pair itself.
     */
    Pair &operator=(const Pair &rhs)
    {
        first = rhs.first;
        second = rhs.second;
        return *this;
    }

    /**
     * Assigns this pair's values to the values contained in another pair of
     * differing, but compatible types. The type compatibility must be aligned
     * (i.e. first converts to first, second converts to second).
     * @tparam A1 Type static castable to first
     * @tparam B1 Type static castable to B
     * @param rhs The pair to copy values from.
     * @return A reference to the pair itself.
     */
    template <typename A1, typename B1>
    Pair &operator=(const Pair<A1, B1> &rhs)
    {
        first = static_cast<A>(rhs.first);
        second = static_cast<B>(rhs.second);
        return *this;
    };

    bool operator==(const Pair &rhs) const
    {
        return first == rhs.first && second == rhs.second;
    }

    bool operator!=(const Pair &rhs) const { return !operator==(rhs); }
};

template <typename A, typename B>
Pair<A, B> MakePair(const A &a, const B &b) { return Pair{a, b}; };

} // libsys namespace

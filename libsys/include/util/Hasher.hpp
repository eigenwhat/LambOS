#pragma once

#include <stddef.h>
#include <stdint.h>

template <typename T> struct Hasher;

template <typename T> struct Hasher<T*>
{
    constexpr size_t operator()(T *p) const { return static_cast<size_t>(p); }
};

/*
 * Unsigned Types
 */

template <> struct Hasher<uint64_t>
{
    constexpr size_t operator()(int64_t i) const
    {
        return static_cast<size_t>(i & SIZE_MAX);
    }
};
template <> struct Hasher<uint8_t> : public Hasher<uint64_t> {};
template <> struct Hasher<uint16_t> : public Hasher<uint64_t> {};
template <> struct Hasher<uint32_t> : public Hasher<uint64_t> {};

/*
 * Signed Types
 */

template <> struct Hasher<int64_t>
{
    constexpr size_t operator()(int64_t i) const
    {
        return static_cast<size_t>(i & SIZE_MAX);
    }
};
template <> struct Hasher<int8_t> : public Hasher<int64_t> {};
template <> struct Hasher<int16_t> : public Hasher<int64_t> {};
template <> struct Hasher<int32_t> : public Hasher<int64_t> {};
template <> struct Hasher<int> : public Hasher<int64_t> {};

/**
 * C string (char*) Hasher. Uses `djb2` algorithm for hashing strings.
 */
template <> struct Hasher<char *>
{
    size_t operator()(char const *str) const
    {
        size_t hash = 5381;
        int c;

        while ((c = *str++))
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

        return hash;
    }
};
template <> struct Hasher<char const *> : public Hasher<char *> {};
#pragma once

#include <util/DynamicArray.hpp>
#include <util/Collection.hpp>
#include <util/Hasher.hpp>

namespace _ns_LIBSYS {
namespace detail {

template <typename T>
constexpr int strcmp(T const *str1, T const *str2)
{
    while (*str1 && *str2 && (*str1++ == *str2++));
    return *(const unsigned char *)str1 - *(const unsigned char *)str2;
}

template <typename T>
constexpr size_t strlen(T const *str)
{
    size_t ret = 0;
    while (str[ret] != 0) ++ret;
    return ret;
}

} // namespace detail

/**
 * A string of character-like objects.
 * @tparam T The character-like type.
 */
template <typename T> class BasicString
{
  public:
    static constexpr size_t npos = static_cast<size_t>(-1);
    using ValueType = T;

    static BasicString const &emptyString()
    {
        static BasicString *s_empty = nullptr;
        if (!s_empty) { s_empty = new BasicString; }
        return *s_empty;
    }

    /** Constructs a BasicString with the default pre-allocation. */
    constexpr BasicString() : BasicString(8) {}

    /**
     * Constructs a BasicString, pre-allocating space for `reserve` number of
     * characters.
     * @param reserve The number of characters to pre-allocate space for.
     */
    constexpr BasicString(size_t reserve) : _data(reserve + 1)
    {
        for (size_t i = 0; i < _data.capacity(); ++i) { _data[i] = 0; }
    }

    /**
     * Constructs a BasicString, allocating and filling space for the provided
     * null-terminated string.
     * @param str The string to initialize as.
     */
    constexpr BasicString(T const *str) : BasicString(str, detail::strlen(str)) {}

    /**
     * Constructs a BasicString, allocating and filling space for the first
     * X characters in the null-terminated string.
     * @param str The string to initialize as.
     * @param strlen The number of characters to read.
     */
    constexpr BasicString(T const *str, size_t strlen) : _data(strlen + 1), _size(strlen)
    {
        if (strlen == 0) {
            _data[0] = 0;
        } else {
            for (size_t i = 0; i < strlen; ++i) {
                _data[i] = str[i];
            }
            _data[_size] = 0; // null terminate
        }
    }

    constexpr BasicString(BasicString const &other) : BasicString(other.cstr()) {}

    /**
     * Returns whether or not the Collection is empty.
     * @return `true` if empty, `false` otherwise.
     */
    [[nodiscard]] constexpr bool isEmpty() const { return _size == 0; }

    /**
     * The number of characters in the BasicString.
     * @return A size_t equal to the number of characters.
     */
    [[nodiscard]] constexpr size_t size() const { return _size; }

    /**
     * Appends a character to the end of the BasicString.

     * @param object The object to add.
     * @return `true` if the BasicString changed. `false` otherwise.
     */
    constexpr bool insert(ValueType const &object)
    {
        if (_size == capacity()) {
            _data.resize();
        }

        _data[_size++] = object;
        _data[_size] = 0; // null terminate
        return true;
    }

    /**
     * Removes the first instance of that character from the BasicString,
     * reducing its size.
     *
     * @param object The value to remove.
     * @return `true` if the Collection changed. `false` otherwise.
     */

    constexpr bool remove(const T &object)
    {
        size_t removalIdx = SIZE_MAX;
        for (size_t i = 0; i < _size; ++i) {
            if (_data[i] == object) {
                removalIdx = i;
                break;
            }
        }

        return remove(removalIdx);
    }

    /**
     * Removes the character at the given index.
     * @note This is O(n), where n is distance from the end of the BasicString.
     * @param idx The index to remove from.
     * @return
     */
    constexpr bool remove(size_t idx)
    {
        if (idx >= _size) {
            return false;
        }

        --_size;
        // move everything right of idx left one.
        for (size_t i = idx; i < _size; ++i) {
            _data[i] = _data[i+1];
        }
        _data[_size] = 0; // null terminate

        return true;
    }

    /**
     * Empties the BasicString.
     * @note This is O(1). The size value and the first element are set to 0,
     * in effect making the string empty, but the data is otherwise left as-is.
     * @return `true`.
     */
    constexpr bool clear()
    {
        _size = 0;
        _data[_size] = 0; // null terminate
        return true;
    }

    /**
     * Returns an immutable null-terminated C string.
     * @return A C string.
     */
    constexpr T const *cstr() const { return &_data[0]; }

    /**
     * The maximum capacity of the currently allocated block.
     * @return The size of the underlying memory block.
     */
    constexpr size_t capacity() const { return _data.capacity(); }

    /**
     * Appends the provided BasicString.
     *
     * @param str The BasicString to append.
     * @return A reference to this.
     */
    constexpr BasicString &append(BasicString const &str)
    {
        return append(str.cstr(), str.size());
    }

    /**
     * Appends the provided null-terminated string.
     *
     * @param str The null-terminated string to append.
     * @return A reference to this.
     */
    constexpr BasicString &append(ValueType const *str)
    {
        return append(str, detail::strlen(str));
    }

    /**
     * Appends a number of characters from the provided null-terminated string.
     *
     * @param str The null-terminated string to append.
     * @param strlen The number of characters to read from it.
     * @return A reference to this.
     */
    constexpr BasicString &append(ValueType const *str, size_t strlen)
    {
        const size_t requiredSize = _size + strlen + 1;
        if (requiredSize > capacity())
        {
            const size_t newSize = requiredSize > capacity()*2 ? requiredSize : 0;
            _data.resize(newSize);
        }

        for (size_t i = 0; i < strlen; ++i) {
            _data[_size+i] = str[i];
        }
        _size += strlen;
        _data[_size] = 0; // null terminate

        return *this;
    }

    /**
     * Appends the provided character.
     * @param ch The character to append.
     * @return A reference to this.
     */
    constexpr BasicString &append(ValueType ch)
    {
        insert(ch);
        return *this;
    }

    /**
     * Returns a substring [start, start+length) as a new BasicString.
     * @param start The start index.
     * @param length The length of the string.
     * @return A new BasicString equal to the substring.
     */
    constexpr BasicString substring(size_t start = 0, size_t length = npos) const
    {
        if (start > _size) { start = _size; }

        if (length > (_size - start)) { length = _size - start; }

        if (length == 0) { return BasicString(); }

        return BasicString(&_data[start], length);
    }

    /**
     * Returns the element at the given index.  No bounds checking is performed.
     * @param idx The index of the object.
     * @return The object at that index. If the index is out of bounds, the
     *         return value is undefined.
     */
    constexpr T& operator[](size_t idx) const { return _data[idx]; }

    /**
     * Returns a BasicString containing this appended by the given BasicString.
     * @param str The BasicString to append.
     * @return A new BasicString that's a concatenation of this and rhs.
     */
    constexpr BasicString operator+(BasicString const &rhs) const
    {
        return BasicString(*this).append(rhs);
    }

    /**
     * Appends the provided character.
     * @param str The character to append.
     * @return A reference to this.
     */
    constexpr BasicString operator+(ValueType rhs) const
    {
        BasicString ret(*this);
        ret.insert(rhs);
        return ret;
    }

    constexpr BasicString &operator=(BasicString const &rhs)
    {
        _size = 0;
        return append(rhs);
    }

    constexpr BasicString &operator=(T const *str)
    {
        _size = 0;
        return append(str);
    }

    /**
     * Appends the provided BasicString.
     *
     * @param str The BasicString to append.
     * @return A reference to this.
     */
    constexpr BasicString &operator+=(BasicString const &rhs) { return append(rhs); }

    /**
     * Appends the provided null-terminated string.
     *
     * @param str The null-terminated string to append.
     * @return A reference to this.
     */
    constexpr BasicString &operator+=(ValueType const *rhs) { return append(rhs); }

    /**
     * Appends the provided character.
     * @param ch The character to append.
     * @return A reference to this.
     */
    constexpr BasicString &operator+=(ValueType rhs) { return append(rhs); }

    /**
     * Equality operator.
     * @param rhs The BasicString to compare to.
     * @return
     */
    constexpr bool operator==(BasicString const &rhs) const
    {
        return !detail::strcmp(cstr(), rhs.cstr());
    }

    constexpr bool operator!=(BasicString const &rhs) const { return !operator==(rhs); }

  private:
    DynamicArray<T> _data;
    size_t _size = 0;
};

} // libsys namespace

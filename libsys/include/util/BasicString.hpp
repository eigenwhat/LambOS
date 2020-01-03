#pragma once

#include <util/DynamicArray.hpp>
#include <util/Collection.hpp>
#include <util/Hasher.hpp>

#include <compare>
#include <cstring>
#include <ranges>

namespace sys {
namespace detail {

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
    using value_type = T;

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
//    constexpr BasicString(size_t reserve) : _data(reserve + 1) { std::ranges::fill(_data, T{0}); }

    /**
     * Constructs a BasicString, allocating and filling space for the provided
     * null-terminated string.
     * @param str The string to initialize as.
     */
    constexpr BasicString(T const *str) : BasicString(str, detail::strlen(str)) {}
    constexpr BasicString &operator=(T const *str) { _size = 0; return append(str); }

    /**
     * Constructs a BasicString, allocating and filling space for the first
     * X characters in the null-terminated string.
     * @param str The string to initialize as.
     * @param len The number of characters to read.
     */
    constexpr BasicString(T const *str, size_t len) : _data(len + 1), _size(len)
    {
        if (len > 0) { std::strncpy(_data.data(), str, len); }
        _data[_size] = 0; // null terminate
    }

    constexpr BasicString(BasicString const &other) : BasicString(other.cstr(), other.size()) {}
    constexpr BasicString &operator=(BasicString const &rhs) { _size = 0; return append(rhs); }

    constexpr BasicString(BasicString &&other) noexcept = default;
    constexpr BasicString &operator=(BasicString &&rhs) noexcept = default;
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
    constexpr bool insert(value_type const &object)
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
    constexpr T const *cstr() const { return _data.data(); }

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
     * Appends the provided range.
     *
     * @param str The BasicString to append.
     * @return A reference to this.
     */
    template <std::input_iterator It>
    constexpr BasicString &append(It begin, It end)
    {
        for (auto i = begin; i != end; ++i) {
            if (_size == capacity()) { _data.resize(); }
            _data[_size++] = *i;
        }

        _data[_size] = 0; // null terminate
        return *this;
    }

    /**
     * Appends the provided null-terminated string.
     *
     * @param str The null-terminated string to append.
     * @return A reference to this.
     */
    constexpr BasicString &append(value_type const *str)
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
    constexpr BasicString &append(value_type const *str, size_t strlen)
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
    constexpr BasicString &append(value_type ch)
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
    constexpr T const & operator[](size_t idx) const { return _data[idx]; }

    /**
     * Returns the element at the given index.  No bounds checking is performed.
     * @param idx The index of the object.
     * @return The object at that index. If the index is out of bounds, the
     *         return value is undefined.
     */
    constexpr T & operator[](size_t idx) { return _data[idx]; }

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
    constexpr BasicString operator+(value_type rhs) const
    {
        BasicString ret(*this);
        ret.insert(rhs);
        return ret;
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
    constexpr BasicString &operator+=(value_type const *rhs) { return append(rhs); }

    /**
     * Appends the provided character.
     * @param ch The character to append.
     * @return A reference to this.
     */
    constexpr BasicString &operator+=(value_type rhs) { return append(rhs); }

    /**
     * Equality operator.
     * @param rhs The BasicString to compare to.
     * @return
     */
    constexpr bool operator==(BasicString const &rhs) const
    {
        return _size == rhs._size && (*this <=> rhs == 0);
    }

    constexpr auto operator<=>(BasicString const &rhs) const
    {
        auto str1 = cstr();
        auto str2 = rhs.cstr();
        while (*str1 && *str2 && (*str1++ == *str2++));
        const auto diff = *(const unsigned char *)str1 - *(const unsigned char *)str2;
        if      (diff < 0)  { return std::strong_ordering::less; }
        else if (diff == 0) { return std::strong_ordering::equal; }
        else                { return std::strong_ordering::greater; }
    }

    T *begin() { return _data.begin(); }
    T *end() { return _data.begin() + _size; }

    T const *begin() const { return _data.begin(); }
    T const *end() const { return _data.begin() + _size; }

    auto rbegin() { return std::make_reverse_iterator(end()); }
    auto rend() { return _data.rend(); }

    auto rbegin() const { return std::make_reverse_iterator(end()); }
    auto rend() const { return _data.rend(); }

  private:
    DynamicArray<T> _data;
    size_t _size = 0;
};

} // libsys namespace

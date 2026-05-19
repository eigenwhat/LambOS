//
// Created by Martin Miralles-Cordal on 05/18/2026.
//

#ifndef LAMBOS_STRINGVIEW_HPP
#define LAMBOS_STRINGVIEW_HPP

#include <util/BasicString.hpp>

#include <algorithm>
#include <cstddef>

namespace sys {

/**
 * A non-mutating view into a string of character-like objects.
 * @tparam T The character-like type.
 */
template <typename T>
class BasicStringView
{
  public:
    static constexpr size_t npos = static_cast<size_t>(-1);
    using value_type = T;
    using const_iterator = T const *;
    using iterator = const_iterator;

    /** Constructs a BasicStringView. */
    constexpr BasicStringView() noexcept = default;
    
    /**
     * Constructs a BasicStringView, allocating and filling space for the provided
     * null-terminated string.
     * @param str The string to initialize as.
     */
    constexpr BasicStringView(T const *str) noexcept : BasicStringView(str, detail::strlen(str)) {}
    constexpr BasicStringView &operator=(T const *str) noexcept
    {
        data_ = str;
        size_ = detail::strlen(str);
        return *this;
    }

    /**
     * Constructs a BasicStringView, allocating and filling space for the first
     * X characters in the null-terminated string.
     * @param str The string to initialize as.
     * @param len The number of characters to read.
     */
    constexpr BasicStringView(T const *str, size_t len) noexcept : data_{str}, size_{len} {}
    constexpr BasicStringView(BasicString<T> const& str) noexcept : data_{str.data()}, size_{str.size()} {}

    constexpr BasicStringView(BasicStringView const& other) = default;
    constexpr BasicStringView &operator=(BasicStringView const& rhs) = default;

    constexpr BasicStringView(BasicStringView&& other) noexcept = default;
    constexpr BasicStringView &operator=(BasicStringView&& rhs) noexcept = default;
    /**
     * Returns whether or not the Collection is empty.
     * @return `true` if empty, `false` otherwise.
     */
    [[nodiscard]] constexpr bool isEmpty() const { return size_ == 0; }

    /**
     * The number of characters in the BasicStringView.
     * @return A size_t equal to the number of characters.
     */
    [[nodiscard]] constexpr size_t size() const { return size_; }

    /**
     * Resets the BasicStringView.
     * @return `true`.
     */
    constexpr bool clear()
    {
        data_ = nullptr;
        size_ = 0;
        return true;
    }

    /**
     * Returns a substring [start, start+length) as a new BasicStringView.
     * @param start The start index.
     * @param length The length of the string.
     * @return A new BasicStringView equal to the substring.
     */
    constexpr BasicStringView substring(size_t start = 0, size_t length = npos) const
    {
        if (start > size_) { start = size_; }

        if (length > (size_ - start)) { length = size_ - start; }

        if (length == 0) { return BasicStringView(); }

        return BasicStringView(&data_[start], length);
    }

    /**
     * Returns the element at the given index.  No bounds checking is performed.
     * @param idx The index of the object.
     * @return The object at that index. If the index is out of bounds, the
     *         return value is undefined.
     */
    constexpr T const& operator[](size_t idx) const { return data_[idx]; }


    /**
     * Equality operator.
     * @param rhs The BasicStringView to compare to.
     * @return
     */
    constexpr bool operator==(BasicStringView const& rhs) const { return size_ == rhs.size_ && (*this <=> rhs == 0); }

    constexpr auto operator<=>(BasicStringView const& rhs) const
    {
        auto diff = strncmp(data(), rhs.data(), std::min(size_, rhs.size_));
        if (diff < 0) { return std::strong_ordering::less; }
        if (diff > 0) { return std::strong_ordering::greater; }
        return std::strong_ordering::equal;
    }

    constexpr bool operator==(BasicString<T> const& rhs) const { return size_ == rhs.size_ && (*this <=> rhs == 0); }
    constexpr auto operator<=>(BasicString<T> const& rhs) const
    {
        auto diff = strncmp(data(), rhs.data(), std::min(size_, rhs.size_));
        if (diff < 0) { return std::strong_ordering::less; }
        if (diff > 0) { return std::strong_ordering::greater; }
        return std::strong_ordering::equal;
    }

    constexpr T const *begin() const { return data_; }
    constexpr T const *end() const { return data_ + size_; }

    constexpr auto rbegin() const { return std::make_reverse_iterator(end()); }
    constexpr auto rend() const { return std::make_reverse_iterator(begin()); }

    constexpr T const *data() const { return data_; }

  private:
    T const *data_ = nullptr;
    std::size_t size_ = 0;
};

using StringView = BasicStringView<char>;

} // namespace sys

#endif // LAMBOS_STRINGVIEW_HPP

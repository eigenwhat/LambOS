//
// Created by Martin Miralles-Cordal on 10/19/2019.
//

#pragma once

#include <util/Concepts.hpp>
#include <util/TypeTraits.hpp>

#include <iterator>
#include <utility>

namespace _ns_LIBSYS {
namespace concepts {

template <typename T>
concept Iterator = std::copy_constructible<T>
                   && std::copy_assignable<T>
                   && std::destructible<T>
                   && std::swappable<T>
                   && Dereferencable<T>
                   && requires(T a) { a++; ++a; };

template <typename T>
concept InputIterator = Pointer<T>
                        || (std::equality_comparable<T> && Iterator<T> && requires(T a)
                           {
                               typename T::value_type;
                               typename T::reference;
                               typename T::pointer;
                               requires requires { { *a } -> std::same_as<typename T::value_type>; }
                                     || requires { { *a } -> std::same_as<typename T::reference>; };
                               { a.operator->() } -> std::same_as<typename T::pointer>;
                           });

template <typename T, typename ValueType>
concept IteratorImpl = std::equality_comparable<T> && requires(T &a)
{
    requires std::same_as<typename T::value_type, ValueType>;
    a.increment();
    a.decrement();
    { a.get_value() } -> std::same_as<ValueType&>;
    { a.get_ptr() } -> std::convertible_to<ValueType*>;
};

template <typename T, typename ValueType>
concept RandomAccessIteratorImpl = IteratorImpl<T, ValueType> && std::totally_ordered<T>
                                   && requires(T &a, T const &b, std::ptrdiff_t offset)
{
    { b.get_value_at_offset(offset) } -> std::same_as<ValueType&>;
    a.jump(offset);
    { b.distance(a) } -> std::same_as<std::ptrdiff_t>;
};

} // namespace concepts

namespace iterator_impl {

template <typename Impl, bool Const> requires concepts::IteratorImpl<Impl, typename Impl::value_type>
struct Bidirectional
{
    using value_type = typename Impl::value_type;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using reference = const_lvref_t<value_type, Const>;
    using pointer = typename std::conditional<Const, value_type const *, value_type *>::type;

    constexpr Bidirectional(const Bidirectional &other) = default;
    constexpr Bidirectional& operator=(const Bidirectional &other) = default;
    constexpr Bidirectional(Bidirectional &&other) noexcept = default;
    constexpr Bidirectional& operator=(Bidirectional &&other) noexcept = default;

    constexpr Bidirectional() = default;

    template <bool OtherConst, typename = std::enable_if_t<Const && !OtherConst>>
    constexpr Bidirectional(const Bidirectional<Impl, OtherConst> &other) : impl_(other.impl_) {}
    template <bool OtherConst, typename = std::enable_if_t<Const && !OtherConst>>
    constexpr Bidirectional& operator=(const Bidirectional<Impl, OtherConst> &other)
    {
        impl_ = (other.impl_);
        return *this;
    }

    constexpr Bidirectional(Impl iter) : impl_{std::move(iter)} {}

    constexpr reference operator*() const { return impl_.get_value(); }
    constexpr pointer operator->() const { return impl_.get_ptr(); }

    constexpr Bidirectional& operator++() { impl_.increment(); return *this; }
    constexpr Bidirectional operator++(int) { auto copy = *this; operator++(); return copy; }
    constexpr Bidirectional& operator--() { impl_.decrement(); return *this; }
    constexpr Bidirectional operator--(int) { auto copy = *this; operator--(); return copy; }

    friend constexpr bool operator==(Bidirectional const &lhs, Bidirectional const &rhs) = default;

    friend void swap(Bidirectional &lhs, Bidirectional &rhs)
    {
        Impl temp = std::move(rhs.impl_);
        rhs.impl_ = std::move(lhs.impl_);
        lhs.impl_ = std::move(temp);
    }

  protected:
    friend struct Bidirectional<Impl, !Const>;
    Impl impl_;
};

}

template <typename Impl, bool Const> requires concepts::IteratorImpl<Impl, typename Impl::value_type>
struct BasicIterator : iterator_impl::Bidirectional<Impl, Const>
{
  private:
    using super = iterator_impl::Bidirectional<Impl, Const>;
  public:
    using value_type = typename super::value_type;
    using iterator_category = typename super::iterator_category;
    using difference_type = typename super::difference_type;
    using reference = typename super::reference;
    using pointer = typename super::pointer;
    using super::super;
    using super::operator=;
};

template <typename Impl, bool Const> requires concepts::RandomAccessIteratorImpl<Impl, typename Impl::value_type>
struct BasicIterator<Impl, Const> : iterator_impl::Bidirectional<Impl, Const>
{
  private:
    using super = iterator_impl::Bidirectional<Impl, Const>;
  public:
    using value_type = typename super::value_type;
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = typename super::difference_type;
    using reference = typename super::reference;
    using pointer = typename super::pointer;

    constexpr BasicIterator() = default;

    template <bool OtherConst, typename = std::enable_if_t<Const && !OtherConst>>
    constexpr BasicIterator(const BasicIterator<Impl, OtherConst> &other) : super(other.impl_) {}
    template <bool OtherConst, typename = std::enable_if_t<Const && !OtherConst>>
    constexpr BasicIterator& operator=(const BasicIterator<Impl, OtherConst> &other)
    {
        super::operator=(other);
        return *this;
    }

    constexpr BasicIterator(Impl iter) : super{std::move(iter)} {}

    constexpr reference operator[](std::size_t idx) const
    {
        return this->impl_.get_value_at_offset(std::ptrdiff_t(idx));
    }

    friend BasicIterator operator+(BasicIterator const &lhs, difference_type rhs)
    {
        auto copy = lhs;
        copy.impl_.jump(rhs);
        return copy;
    }
    friend BasicIterator operator+(difference_type lhs, BasicIterator const &rhs) { return rhs + lhs; }
    BasicIterator& operator+=(difference_type offset) { this->impl_.jump(offset); return *this; }
    friend BasicIterator operator-(BasicIterator const &lhs, difference_type rhs) { return lhs + (-rhs); }
    BasicIterator& operator-=(difference_type offset) { return operator+=(-offset); }

    template <bool OtherConst, typename = std::enable_if_t<Const && !OtherConst>>
    difference_type operator-(BasicIterator<Impl, OtherConst> const &rhs) const
    {
        return this->impl_.distance(rhs.impl_);
    }

    constexpr bool operator<=>(BasicIterator const &rhs) const = default;
};

template <typename Impl, bool Const>
constexpr BasicIterator<Impl, Const>&
operator++(BasicIterator<Impl, Const> &rhs) { rhs.operator++(); return rhs; }

template <typename Impl, bool Const>
constexpr BasicIterator<Impl, Const>
operator++(BasicIterator<Impl, Const> &lhs, int) { auto copy = lhs; ++lhs; return copy; }

template <typename Impl, bool Const>
constexpr BasicIterator<Impl, Const>&
operator--(BasicIterator<Impl, Const> &rhs) { rhs.operator--(); return rhs; }

template <typename Impl, bool Const>
constexpr BasicIterator<Impl, Const>
operator--(BasicIterator<Impl, Const> &lhs, int) { auto copy = lhs; --lhs; return copy; }

template <typename Impl> using Iterator = BasicIterator<Impl, false>;
template <typename Impl> using ConstIterator = BasicIterator<Impl, true>;

} // libsys namespace

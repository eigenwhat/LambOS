#pragma once

#include <util/Concepts.hpp>
#include <util/TypeTraits.hpp>

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

template <typename T, typename value_type>
concept IteratorImpl = std::equality_comparable<T> && requires(T a)
{
    requires std::same_as<typename T::value_type, value_type>;
    a.increment();
    a.decrement();
    { std::move(a.get_value()) } -> std::convertible_to<value_type>;
    { a.get_ptr() } -> std::convertible_to<value_type*>;
};

} // namespace concepts

template <typename Impl, bool Const> requires concepts::IteratorImpl<Impl, typename Impl::value_type>
struct BasicIterator
{
    using value_type = typename Impl::value_type;
    static_assert(concepts::IteratorImpl<Impl, value_type>, "Impl is not compatible with BasicIterator");

    using difference_type = std::ptrdiff_t;
    using reference = const_lvref_t<value_type, Const>;
    using pointer = typename std::conditional<Const, value_type const *, value_type *>::type;

    constexpr BasicIterator(const BasicIterator &other) = default;
    constexpr BasicIterator& operator=(const BasicIterator &other) = default;
    constexpr BasicIterator(BasicIterator &&other) noexcept = default;
    constexpr BasicIterator& operator=(BasicIterator &&other) noexcept = default;

    template <bool OtherConst, typename = std::enable_if_t<Const && !OtherConst>>
    constexpr BasicIterator(const BasicIterator<Impl, OtherConst> &other) : impl_(other.impl_) {}
    constexpr BasicIterator(Impl iter) : impl_{std::move(iter)} {}

    constexpr reference operator*() const { return impl_.get_value(); }
    constexpr pointer operator->() const { return impl_.get_ptr(); }

    constexpr BasicIterator& operator++() { impl_.increment(); return *this; }
    constexpr BasicIterator operator++(int) { auto copy = *this; operator++(); return copy; }
    constexpr BasicIterator& operator--() { impl_.decrement(); return *this; }
    constexpr BasicIterator operator--(int) { auto copy = *this; operator--(); return copy; }

    constexpr bool operator==(BasicIterator const &rhs) const { return impl_ == rhs.impl_; }
    constexpr bool operator!=(BasicIterator const &rhs) const { return !(rhs == *this); }

    friend void swap(BasicIterator &lhs, BasicIterator &rhs)
    {
        Impl temp = std::move(rhs.impl_);
        rhs.impl_ = std::move(lhs.impl_);
        lhs.impl_ = std::move(temp);
    }

  private:
    friend struct BasicIterator<Impl, !Const>;
    Impl impl_;
};

template <typename Impl> using Iterator = BasicIterator<Impl, false>;
template <typename Impl> using ConstIterator = BasicIterator<Impl, true>;

} // libsys namespace

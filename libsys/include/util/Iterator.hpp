#pragma once

#include <util/Concepts.hpp>
#include <util/TypeTraits.hpp>

#include <utility>

namespace concepts {

template <typename T>
concept Iterator = CopyConstructible<T> && CopyAssignable<T> && Destructible<T> && Swappable<T> && requires(T a)
{
    { a++ };
    { ++a };
    { *a };
};

template <typename T>
concept InputIterator = Pointer<T>
                        || (EqualityComparable<T> && Iterator<T> && requires(T a)
                           {
                               typename T::ValueType;
                               typename T::reference;
                               typename T::pointer;
                               requires requires { { *a } -> typename T::ValueType; }
                                     || requires { { *a } -> typename T::reference; };
                               { a.operator->() } -> typename T::pointer;
                           });

template <typename T, typename ValueType>
concept IteratorImpl = EqualityComparable<T> && requires(T a)
{
    typename T::ValueType;
    { a.increment() } -> Convertible<void>;
    { a.decrement() } -> Convertible<void>;
    { a.get_value() } -> Convertible<ValueType>;
    { a.get_ptr() } -> Convertible<ValueType*>;
};

} // namespace concepts

template <typename Impl, bool Const> requires concepts::IteratorImpl<Impl, typename Impl::ValueType>
struct BasicIterator
{
    using ValueType = typename Impl::ValueType;
    static_assert(concepts::IteratorImpl<Impl, ValueType>, "Impl is not compatible with BasicIterator");

    using difference_type = std::ptrdiff_t;
    using reference = const_lvref_t<ValueType, Const>;
    using pointer = typename std::conditional<Const, ValueType const *, ValueType *>::type;

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
    friend class BasicIterator<Impl, !Const>;
    using other_t = BasicIterator<Impl, !Const>;

    Impl impl_;
};

template <typename Impl> using Iterator = BasicIterator<Impl, false>;
template <typename Impl> using ConstIterator = BasicIterator<Impl, true>;

//
// Created by Martin Miralles-Cordal on 5/16/2018.
//

#pragma once

#include <util/TypeTraits.hpp>
#include <util/Void.hpp>

#include <concepts>
#include <cstdint>
#include <functional>
#include <iterator>

namespace sys {

template <typename T>
class Maybe
{
  public:
    // our custom types
    using RegularType = regular_t<T>;
    template <bool Const> struct BasicIterator;
    using RawType = typename std::decay<RegularType>::type;
    using MutableType = typename std::remove_const<RegularType>::type;

    // std container member types
    using iterator = BasicIterator<false>;
    using const_iterator = BasicIterator<true>;
    using value_type = std::remove_reference_t<RegularType>;
    using reference = lvref_t<value_type>;
    using const_reference = cref_t<value_type>;
    using rvalue_reference = rvref_t<value_type>;
    using pointer = value_type *;
    using const_pointer = value_type const *;
    using difference_type = int64_t;
    using size_type = size_t;

    constexpr Maybe() = default;
    constexpr Maybe(Void) : Maybe() {}

    template <std::copy_constructible U = T>
    constexpr Maybe(T const &v) : _store(v), _set(true) {}

    template <std::move_constructible U = T>
    constexpr Maybe(T &&v) : _store(std::move(v)), _set(true) {}

    constexpr Maybe(Maybe const &other) : _store{}, _set(other._set) { if (_set) { _store.init(other.ref_()); } }

    /** Copy assignment operator */
    Maybe& operator=(Maybe const &other)
    {
        if (other._set) { set(other.ref_()); }
        else { clear(); }
        return *this;
    }

    /** Move constructor. */
    constexpr Maybe(Maybe&& other) : _store{}, _set(other._set)
    {
        if (_set) { _store.init(std::move(other).ref_()); other.clear(); }
    }

    /** Move assignment operator */
    Maybe& operator=(Maybe &&other)
    {
        if (other._set) { set(std::move(other).ref_()); other.clear(); }
        else { clear(); }
        return *this;
    }

    /** Clears the stored value. */
    constexpr void clear() { _store.destroy(_set); _set = false; }

    /** Returns whether this contains a value. */
    constexpr operator bool() const noexcept { return _set; }

    /** Returns whether this contains a value. */
    constexpr bool isPresent() const noexcept { return _set; }

    /**
     * Accesses the contained value.
     * @return A const pointer to the value if set, nullptr otherwise.
     */
    constexpr value_type const * operator->() const { return _set ? &ref_() : nullptr; }

    /**
     * Accesses the contained value.
     * @return A pointer to the value if set, nullptr otherwise.
     */
    constexpr value_type * operator->() { return _set ? &ref_() : nullptr; }

    /**
     * Accesses the contained value.
     * @return A const ref to the value. Return value is undefined if not set.
     */
    constexpr value_type const & operator*() const { return ref_(); }

    /**
     * Accesses the contained value.
     * @return A reference to the value. Return value is undefined if not set.
     */
    constexpr value_type & operator*() { return ref_(); }

    /**
     * Returns the contained value.
     * @return A const ref to the value. Return value is undefined if not set.
     */
    constexpr value_type const & get() const { return ref_(); }

    /**
     * Returns the contained value.
     * @return A reference to the value. Return value is undefined if not set.
     */
    constexpr value_type & get() { return ref_(); }

    /**
     * Returns a copy of the contained value, or a default value if not set.
     * @param other The value to default to.
     * @return A copy of the value, or the default value if this is not set.
     */
    constexpr value_type getOr(T &&other) const &
    {
        return _set ? ref_() : std::forward<T>(other);
    }

    /**
     * Moves out the contained value or returns a default value if not set.
     * @param other The value to default to.
     * @return A copy of the value, or the default value if this is not set.
     */
    constexpr value_type getOr(T &&other) &&
    {
        return _set ? std::move(*this).ref_() : std::forward<T>(other);
    }
    
    /** Sets the Maybe to a copy of the given value. */
    constexpr void set(const_reference value)
    {
        if (_set) { _store.reassign(value); }
        else { _store.init(value); }
        _set = true;
    }

    /** Sets the Maybe by moving the given value into the storage. */
    constexpr void set(rvalue_reference value)
    {
        if (_set) { _store.reassign(std::move(value)); }
        else { _store.init(std::move(value)); }
        _set = true;
    }

    /** @name STL container interface */
    /** @{ */

    constexpr bool empty() const { return empty(); }
    constexpr size_type size() const { return isPresent() ? 1 : 0; }
    constexpr size_type max_size() const { return 1; }

    constexpr const_iterator cbegin() const { return isPresent() ? const_iterator{this} : cend(); }
    constexpr const_iterator cend() const { return const_iterator{this, const_iterator::kEndIterator}; }
    constexpr const_iterator begin() const { return cbegin(); }
    constexpr const_iterator end() const { return cend(); }
    constexpr iterator begin() { return isPresent() ? iterator{this} : end(); }
    constexpr iterator end() { return iterator{this, iterator::kEndIterator}; }

    constexpr reference front() { return get(); }
    constexpr const_reference front() const { return get(); }
    constexpr reference back() { return get(); }
    constexpr const_reference back() const { return get(); }

    /** @} */

  private:
    inline constexpr decltype(auto) ref_() && noexcept { return std::move(_store).ref(); }
    inline constexpr decltype(auto) ref_() const && noexcept { return std::move(_store).ref(); }
    inline constexpr decltype(auto) ref_() &  noexcept{ return _store.ref(); }
    inline constexpr decltype(auto) ref_() const & noexcept{ return _store.ref(); }

    template<typename Ts>
    union Storage
    {
        constexpr Storage() : _nothing() {}
        constexpr Storage(Ts const &aValue) : _value(aValue) {}
        constexpr Storage(Ts &&aValue) : _value(std::move(aValue)) {}
        ~Storage() {}

        constexpr void destroy(bool isSet) { if (isSet) { _value.~Ts(); } }
        constexpr void init(const Ts &val) { new (&_value) Ts{val}; }
        constexpr void init(Ts &&val) { new (&_value) Ts{std::move(val)}; }

        template <std::same_as<Ts> Us = Ts> requires std::copy_assignable<Us>
        constexpr void reassign(const Ts &val) { _value = val; }

        template <std::same_as<Ts> Us = Ts> requires (!std::copy_assignable<Us> && std::copy_constructible<Us>)
        constexpr void reassign(const Ts &val) { destroy(true); init(val); }

        template <std::same_as<Ts> Us = Ts> requires std::move_assignable<Us>
        constexpr void reassign(Ts &&val) { _value = std::move(val); }

        template <std::same_as<Ts> Us = Ts> requires (std::move_constructible<Us> && !std::move_assignable<Us>)
        constexpr void reassign(Ts &&val) { destroy(true); init(std::move(val)); }

        constexpr value_type&& ref() && noexcept { return std::move(_value); }
        constexpr value_type&& ref() const && noexcept { return std::move(_value); }

        constexpr value_type& ref() & noexcept { return _value; }
        constexpr value_type const & ref() const & noexcept { return _value; }

      private:
        Void _nothing;
        Ts _value;
    };

    template<typename Ts>
    union Storage<Ts &>
    {
        constexpr Storage()  noexcept : _nothing() {}
        constexpr Storage(reference aValue) noexcept : _value(&aValue) {}

        constexpr void destroy(bool) noexcept {}
        constexpr void init(reference val) noexcept { _value = val; }
        void reassign(reference val) noexcept { _value = val; }

        constexpr value_type& ref() const && noexcept { return _value.get(); }
        constexpr value_type& ref() const & noexcept { return _value.get(); }

      private:
        Void _nothing;
        std::reference_wrapper<value_type> _value;
    };

    Storage<MutableType> _store{};
    bool _set{false};

  public:
    template <bool Const>
    struct BasicIterator
    {
        static constexpr const bool kValueIterator = false;
        static constexpr const bool kEndIterator = true;
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = RegularType;
        using difference_type = Maybe::difference_type;
        using reference = const_t<RegularType, Const>;
        using pointer = typename std::conditional<Const, RegularType const *, RegularType *>::type;

        // maintain is_trivially_copy_constructible by defining the Iterator->ConstIterator constructor as a template.
        template <bool OtherConst, typename = typename std::enable_if<Const || !OtherConst>::type>
        constexpr BasicIterator(const BasicIterator<OtherConst> &other) : parent_(other.parent_), end_(other.end_) {}

        explicit BasicIterator(const_ptr_t<Maybe, Const> parent) : parent_(parent), end_(kValueIterator) {}
        constexpr BasicIterator(const_ptr_t<Maybe, Const> parent, bool end) : parent_(parent), end_(end) {}

        constexpr reference operator*() const { return parent_->GetValue(); }

        constexpr BasicIterator& operator++() { end_ = true; return *this; }
        constexpr BasicIterator operator++(int) { auto copy = *this; operator++(); return copy; }
        constexpr BasicIterator& operator--() { end_ = false; return *this; }
        constexpr BasicIterator operator--(int) { auto copy = *this; operator--(); return copy; }

        constexpr bool operator==(BasicIterator const &rhs) const { return parent_ == rhs.parent_ && end_ == rhs.end_; }
        constexpr bool operator!=(BasicIterator const &rhs) const { return !(rhs == *this); }

        constexpr bool end() const { return end_; }

      private:
        const_ptr_t<Maybe, Const> parent_;
        bool end_;
    };
};

template <typename T> Maybe(T const &) -> Maybe<T>;
template <typename T> Maybe(T &&) -> Maybe<T>;

/**
 * Constructs and "promotes" an object to a Maybe<T>.
 * @tparam T The type of Maybe.
 * @tparam Args The types for one of T's constructors. (deducible)
 * @param args The arguments for one of T's constructors.
 * @return The Maybe.
 */
template <typename T, typename... Args>
constexpr static Maybe<T> AsMaybe(Args && ...args)
{
    return Maybe<T>(T(std::forward<Args>(args)...));
}

} // libsys namespace

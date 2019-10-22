//
// Created by Martin Miralles-Cordal on 10/21/19.
//

#pragma once

namespace std {

template<typename T, T _value>
struct __integral_constant
{
    using type = __integral_constant;
    using value_type = T;

    static constexpr T value = _value;

    constexpr operator value_type() const noexcept { return value; }
    constexpr value_type operator()() const noexcept { return value; }
};

template<typename T, T _value> constexpr T __integral_constant<T, _value>::value;
template <bool Value> using __bool_constant = __integral_constant<bool, Value>;

using __true_type = __bool_constant<true>;
using __false_type = __bool_constant<false>;

template <typename, typename> struct __are_same : public __false_type {};
template <typename T> struct __are_same<T, T> : public __true_type {};
template <typename T, typename U> constexpr inline bool __are_same_v = __are_same<T, U>::value;

} // namespace std

//
// Created by Martin Miralles-Cordal on 12/22/2019.
//

#pragma once

#include <bits/algo_support.hpp>

namespace std {

template <typename _ICatT, typename _T, typename _DiffT = ptrdiff_t, typename _PtrT = _T*, typename _RefT = _T&>
struct iterator
{
    using iterator_category = _ICatT;
    using value_type        = _T;
    using difference_type   = _DiffT;
    using pointer           = _PtrT;
    using reference         = _RefT;
};

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : input_iterator_tag {};
struct bidirectional_iterator_tag : forward_iterator_tag {};
struct random_access_iterator_tag : bidirectional_iterator_tag {};
struct contiguous_iterator_tag : random_access_iterator_tag {};

template<typename _T>
struct iterator_traits
{
  using iterator_category = typename _T::iterator_category;
  using value_type        = typename _T::value_type;
  using difference_type   = typename _T::difference_type;
  using pointer           = typename _T::pointer;
  using reference         = typename _T::reference;
};

template<typename _T> requires is_object_v<_T>
struct iterator_traits<_T*>
{
    using iterator_concept  = contiguous_iterator_tag;
    using iterator_category = random_access_iterator_tag;
    using value_type        = remove_cv_t<_T>;
    using difference_type   = ptrdiff_t;
    using pointer           = _T*;
    using reference         = _T&;
};

template<class T> using __with_reference = T&;
template<class T> concept __Referenceable   = requires { typename __with_reference<T>; };
template<class T> concept __Dereferenceable = requires(T& t) {
    { *t } -> __Referenceable;
};

// associated types
// incrementable traits
template<class> struct incrementable_traits {};

template<class T> requires is_object_v<T>
struct incrementable_traits<T*> { using difference_type = ptrdiff_t; };

template<class I> struct incrementable_traits<const I> : incrementable_traits<I> {};

template<class T>
requires requires { typename T::difference_type; }
struct incrementable_traits<T> { using difference_type = typename T::difference_type; };

template<class T>
requires (!requires { typename T::difference_type; } &&
          requires(const T& a, const T& b) { { a - b } -> integral; })
struct incrementable_traits<T> { using difference_type = make_signed_t<decltype(declval<T>() - declval<T>())>; };

// readable traits
template<class> struct __cond_value_type {};

template<class T> requires is_object_v<T>
struct __cond_value_type<T> { using value_type = remove_cv_t<T>; };

template<class> struct readable_traits {};
template<class T> struct readable_traits<T*> : __cond_value_type<T> {};
template<class I> struct readable_traits<const I> : readable_traits<I> {};

template<class I> requires is_array_v<I>
struct readable_traits<I> { using value_type = remove_cv_t<remove_extent_t<I>>; };

template<class T> requires requires { typename T::value_type; }
struct readable_traits<T> : __cond_value_type<typename T::value_type> {};

template<class T> requires requires { typename T::element_type; }
struct readable_traits<T> : __cond_value_type<typename T::element_type> {};


// iterator traits
template<class I> struct iterator_traits;
template<class T> requires is_object_v<T> struct iterator_traits<T*>;

template<class T> using iter_value_t = typename readable_traits<T>::value_type;
template<__Dereferenceable T> using iter_reference_t = decltype(*declval<T&>());
template<class T> using iter_difference_t = typename incrementable_traits<T>::difference_type;

namespace ranges {

namespace __detail {

template<typename _T>
concept __has_adl_iter_move = (is_class_v<_T> || is_union_v<_T> || is_enum_v<_T>) && requires(_T&& __t) {
    iter_move(static_cast<_T&&>(__t));
};

struct __iter_move_fn
{
  private:
    template<typename _T>
    static constexpr bool _Is_noexcept()
    {
        if constexpr (__has_adl_iter_move<_T>) { return noexcept(iter_move(std::declval<_T>())); }
        else { return noexcept(*std::declval<_T>()); }
    }

  public:
    template<typename _T>
    requires __has_adl_iter_move<_T> || requires(_T& __e) { *__e; }
    constexpr decltype(auto)
    operator()(_T&& __e) const noexcept(_Is_noexcept<_T>())
    {
        if constexpr (__has_adl_iter_move<_T>) { return iter_move(static_cast<_T&&>(__e)); }
        else if constexpr (is_reference_v<iter_reference_t<_T>>) { return std::move(*__e); }
        else { return *__e; }
    }
};

} // namespace __detail

__DEF_NIEBLOID(__detail::__iter_move_fn, iter_move)

} // namespace ranges

template<__Dereferenceable T>
requires requires(T& t) { { ranges::iter_move(t) } -> __Referenceable; }
using iter_rvalue_reference_t = decltype(ranges::iter_move(declval<T&>()));

} // namespace std

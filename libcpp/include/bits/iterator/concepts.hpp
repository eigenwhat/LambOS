//
// Created by Martin Miralles-Cordal on 12/22/2019.
//

#pragma once

#include <concepts>
#include <utility>

#include <bits/iterator/types.hpp>

namespace std {

// iterator concepts
// concept readable
template<class In>
concept readable = requires {
                       typename iter_value_t<In>;
                       typename iter_reference_t<In>;
                       typename iter_rvalue_reference_t<In>;
                   }
                   && common_reference_with<iter_reference_t<In>&&, iter_value_t<In>&>
                   && common_reference_with<iter_reference_t<In>&&, iter_rvalue_reference_t<In>&&>
                   && common_reference_with<iter_rvalue_reference_t<In>&&, const iter_value_t<In>&>;

template<readable T>
using iter_common_reference_t = common_reference_t<iter_reference_t<T>, iter_value_t<T>&>;

// concept writable
template<class Out, class T>
concept writable = requires(Out&& o, T&& t) {
    *o = std::forward<T>(t);
    *std::forward<Out>(o) = std::forward<T>(t);
    const_cast<const iter_reference_t<Out>&&>(*o) = std::forward<T>(t);
    const_cast<const iter_reference_t<Out>&&>(*std::forward<Out>(o)) = std::forward<T>(t);
};

// concept weakly_incrementable
template<class I>
concept weakly_incrementable = default_constructible<I> && movable<I> && requires(I i) {
    typename iter_difference_t<I>;
    requires signed_integral<iter_difference_t<I>>;
{ ++i } -> same_as<I&>;
i++;
};

// concept incrementable
template<class I>
concept incrementable = regular<I> && weakly_incrementable<I> && requires(I i) {
    { i++ } -> same_as<I>;
};

// concept input_or_output_iterator
template <typename T> concept __referenceable = __is_referenceable<T>::value;
template<class I>
concept input_or_output_iterator = requires(I i) { { *i } -> __referenceable; } && weakly_incrementable<I>;

// concept sentinel_for
template<class S, class I>
concept sentinel_for = semiregular<S> && input_or_output_iterator<I> && __weakly_equality_comparable_with<S, I>;

// concept sized_sentinel_for
template<class S, class I>
inline constexpr bool disable_sized_sentinel = false;

template<class S, class I>
concept sized_sentinel_for = sentinel_for<S, I> && !disable_sized_sentinel<remove_cv_t<S>, remove_cv_t<I>>
&& requires(const I& i, const S& s) {
    { s - i } -> same_as<iter_difference_t<I>>;
    { i - s } -> same_as<iter_difference_t<I>>;
};

// concept input_iterator
template<class I>
concept input_iterator = input_or_output_iterator<I> && readable<I>
                         && requires { typename iterator_traits<I>::iterator_category; }
                         && derived_from<typename iterator_traits<I>::iterator_category, input_iterator_tag>;

// concept output_iterator
template<class I, class T>
concept output_iterator = input_or_output_iterator<I> && writable<I, T> && requires(I i, T&& t) {
    *i++ = std::forward<T>(t);
};

// concept forward_iterator
template<class I>
concept forward_iterator = input_iterator<I>
                           && derived_from<typename iterator_traits<I>::iterator_category, forward_iterator_tag>
                           && incrementable<I> && sentinel_for<I, I>;

// concept bidirectional_iterator
template<class I>
concept bidirectional_iterator = forward_iterator<I>
                                 && derived_from<typename iterator_traits<I>::iterator_category,
                                                 bidirectional_iterator_tag>
                                 && requires(I i) {
                                     { --i } -> same_as<I&>;
                                     { i-- } -> same_as<I>;
                                 };

// concept random_access_iterator
template<class I>
concept random_access_iterator = bidirectional_iterator<I>
                                 && derived_from<typename iterator_traits<I>::iterator_category,
                                                 random_access_iterator_tag>
                                 && totally_ordered<I>// && sized_sentinel_for<I, I>
                                 && requires(I i, const I j, const iter_difference_t<I> n) {
                                     { i += n } -> same_as<I&>;
                                     { j +  n } -> same_as<I>;
                                     { n +  j } -> same_as<I>;
                                     { i -= n } -> same_as<I&>;
                                     { j -  n } -> same_as<I>;
                                     {  j[n]  } -> same_as<iter_reference_t<I>>;
                                 };

// concept contiguous_iterator
template<class I>
concept contiguous_iterator = random_access_iterator<I>
                              && derived_from<typename iterator_traits<I>::iterator_category, contiguous_iterator_tag>
                              && is_lvalue_reference_v<iter_reference_t<I>>
                              && same_as<iter_value_t<I>, remove_cvref_t<iter_reference_t<I>>>
                              && requires(const I& i) {
                                  { to_address(i) } -> same_as<add_pointer_t<iter_reference_t<I>>>;
                              };

// indirect callable requirements
// indirect callables
//template<class F, class I>
//concept indirectly_unary_invocable = readable<I> && copy_constructible<F> && invocable<F&, iter_value_t<I>&>
//                                  && invocable<F&, iter_reference_t<I>> && invocable<F&, iter_common_reference_t<I>>
//                                  && common_reference_with<invoke_result_t<F&, iter_value_t<I>&>,
//                                                           invoke_result_t<F&, iter_reference_t<I>>>;
//template<class F, class I>
//concept indirectly_regular_unary_invocable = readable<I> && copy_constructible<F>
//                                          && regular_invocable<F&, iter_value_t<I>&>
//                                          && regular_invocable<F&, iter_reference_t<I>>
//                                          && regular_invocable<F&, iter_common_reference_t<I>>
//                                          && common_reference_with<invoke_result_t<F&, iter_value_t<I>&>,
//                                                                   invoke_result_t<F&, iter_reference_t<I>>>;
//
//template<class F, class I>
//concept indirect_unary_predicate = readable<I> && copy_constructible<F> && predicate<F&, iter_value_t<I>&>
//                                && predicate<F&, iter_reference_t<I>> && predicate<F&, iter_common_reference_t<I>>;
//
//template<class F, class I1, class I2 = I1>
//concept indirect_relation = readable<I1> && readable<I2> && copy_constructible<F>
//                         && relation<F&, iter_value_t<I1>&, iter_value_t<I2>&>
//                         && relation<F&, iter_value_t<I1>&, iter_reference_t<I2>>
//                         && relation<F&, iter_reference_t<I1>, iter_value_t<I2>&>
//                         && relation<F&, iter_reference_t<I1>, iter_reference_t<I2>>
//                         && relation<F&, iter_common_reference_t<I1>, iter_common_reference_t<I2>>;
//
//template<class F, class I1, class I2 = I1>
//concept indirect_strict_weak_order = readable<I1> && readable<I2> && copy_constructible<F>
//                                  && strict_weak_order<F&, iter_value_t<I1>&, iter_value_t<I2>&>
//                                  && strict_weak_order<F&, iter_value_t<I1>&, iter_reference_t<I2>>
//                                  && strict_weak_order<F&, iter_reference_t<I1>, iter_value_t<I2>&>
//                                  && strict_weak_order<F&, iter_reference_t<I1>, iter_reference_t<I2>>
//                                  && strict_weak_order<F&, iter_common_reference_t<I1>, iter_common_reference_t<I2>>;
//
//template<class F, class... Is>
//requires (readable<Is> && ...) && invocable<F, iter_reference_t<Is>...>
//using indirect_result_t = invoke_result_t<F, iter_reference_t<Is>...>;

} // namespace std

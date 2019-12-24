//
// Created by Martin Miralles-Cordal on 12/22/2019.
//

#pragma once

#include <concepts>
#include <iterator>
#include <bits/ranges/identity.hpp>

namespace std::ranges {

using std::begin;
using std::cbegin;
using std::rbegin;
using std::crbegin;
using std::end;
using std::cend;
using std::rend;
using std::crend;

struct dangling
{
    constexpr dangling() noexcept = default;
    template <typename... Args> constexpr dangling(Args&&...) noexcept {}
};

template<typename T>
concept range = requires(T& t) { ranges::begin(t); ranges::end(t); };

template <range T>
using iterator_t = decltype(ranges::begin(declval<T&>()));

template <range T>
using sentinel_t = decltype(ranges::end(declval<T&>()));

template <range T>
using range_difference_t = iter_difference_t<ranges::iterator_t<T>>;

template <range T>
using range_value_t = iter_value_t<ranges::iterator_t<T>>;

template <range T>
using range_reference_t = iter_reference_t<ranges::iterator_t<T>>;

template <range T>
using range_rvalue_reference_t = iter_rvalue_reference_t<ranges::iterator_t<T>>;

template<class R, class T> concept output_range = range<R> && output_iterator<iterator_t<R>, T>;
template<typename T> concept input_range = range<T> && input_iterator<iterator_t<T>>;
template<typename T> concept forward_range = input_range<T> && forward_iterator<iterator_t<T>>;
template<typename T> concept bidirectional_range = forward_range<T> && bidirectional_iterator<iterator_t<T>>;
template<typename T> concept random_access_range = bidirectional_range<T> && random_access_iterator<iterator_t<T>>;

template<typename T>
concept common_range = range<T> && same_as<iterator_t<T>, sentinel_t<T>>;

template<range R> inline constexpr bool enable_safe_range = false;

template<class R>
concept safe_range = range<R>
                     && (is_lvalue_reference_v<R> || ranges::enable_safe_range<remove_cvref_t<R>>);

template<range R>
using safe_iterator_t = conditional_t<ranges::safe_range<R>, ranges::iterator_t<R>, ranges::dangling>;

template <class F, class... Args>
concept regular_invocable = invocable<F, Args...>;

template <class F, class I>
concept indirect_unary_invocable = readable<I> && copy_constructible<F>
                                   && invocable<F&, iter_value_t<I>&>
                                   && invocable<F&, iter_reference_t<I>>
                                   && invocable<F&, iter_common_reference_t<I>>
                                   && common_reference_with<invoke_result_t<F, iter_value_t<I>>,
                                                            invoke_result_t<F, iter_reference_t<I>>>;

template <class F, class I>
concept indirect_regular_unary_invocable = readable<I> && copy_constructible<F>
                                           && regular_invocable<F&, iter_value_t<I>&>
                                           && regular_invocable<F&, iter_reference_t<I>>
                                           && regular_invocable<F&, iter_common_reference_t<I>>
                                           && common_reference_with<invoke_result_t<F, iter_value_t<I>>,
                                                                    invoke_result_t<F, iter_reference_t<I>>>;


template <typename F, typename... Is> requires (readable<Is> && ...) && invocable<F, iter_reference_t <Is>...>
using indirect_result_t = invoke_result_t<F, iter_reference_t<Is>...>;

template <typename F, typename... Args>
concept predicate = regular_invocable<F, Args...> && boolean<invoke_result_t<F, Args...>>;

template <typename F, typename I>
concept indirect_unary_predicate = readable<I> && copy_constructible<F> 
                                   && predicate<F&, iter_value_t<I>&>
                                   && predicate<F&, iter_reference_t<I>>
                                   && predicate<F&, iter_common_reference_t<I>>;

template <typename R, typename T, typename U>
concept relation = predicate<R, T, T> && predicate<R, U, U> && predicate<R, T, U> && predicate<R, U, T>;

template <typename F, typename I1, typename I2>
concept indirect_relation = readable<I1> && readable<I2> && copy_constructible<F>
                            && relation<F&, iter_value_t<I1>&, iter_value_t<I2>&>
                            && relation<F&, iter_value_t<I1>&, iter_reference_t<I2>>
                            && relation<F&, iter_reference_t<I1>, iter_value_t<I2>&>
                            && relation<F&, iter_reference_t<I1>, iter_reference_t<I2>>
                            && relation<F&, iter_common_reference_t<I1>, iter_common_reference_t<I2>>;

template <typename I, typename O>
concept indirectly_copyable = readable<I> && writable<O, range_reference_t<I>>;

template <typename I, typename Proj>
struct __projected_helper;

template <readable I, indirect_regular_unary_invocable<I> Proj>
struct __projected_helper<I, Proj> {
    using value_type = remove_cv_t<remove_reference_t<indirect_result_t<Proj&, I>>>;
    indirect_result_t<Proj&, I> operator*() const;
};

template <typename I, typename Proj>
using projected = conditional_t<is_same_v<Proj, identity>, I, __projected_helper<I, Proj>>;

} // namespace std::ranges
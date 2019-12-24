//
// Created by Martin Miralles-Cordal on 12/24/2019.
//

#pragma once

#include <bits/memory/destroy.hpp>
#include <bits/ranges/algorithms/copy.hpp>

namespace std {

template <typename I, typename O>
using uninitialized_copy_result = ranges::copy_result<I, O>;

namespace detail {

struct uninitialized_copy_fn
{
public:
    // Four-legged
    template <input_iterator I, sentinel_for<I> S, nothrow_forward_iterator O, nothrow_sentinel_for<O> S2>
    requires constructible_from<iter_value_t<O>, iter_reference_t<I>>
    uninitialized_copy_result<I, O> operator()(I ifirst, S ilast, O ofirst, S2 olast) const
    {
        return uninitialized_copy_fn::impl4(
                std::move(ifirst), std::move(ilast),
                std::move(ofirst), std::move(olast));
    }

    // Two ranges
    template <ranges::input_range IRng, nothrow_forward_range ORng>
    requires constructible_from<iter_value_t<ranges::iterator_t<ORng>>, iter_reference_t<ranges::iterator_t<IRng>>>
    uninitialized_copy_result<ranges::safe_iterator_t<IRng>, ranges::safe_iterator_t<ORng>>
    operator()(IRng&& irng, ORng&& orng) const
    {
        return uninitialized_copy_fn::impl4(
                begin(irng), end(irng),
                begin(orng), end(orng));
    }

    // Three-legged
    template <input_iterator I, sentinel_for<I> S, nothrow_forward_iterator O>
    requires constructible_from<iter_value_t<O>, iter_reference_t<I>>
    uninitialized_copy_result<I, O> operator()(I ifirst, S ilast, O ofirst) const
    {
        return uninitialized_copy_fn::impl3(std::move(ifirst), std::move(ilast),
                                            std::move(ofirst));
    }

    // Range and a half
    template <ranges::input_range IRng, typename O>
    requires nothrow_forward_iterator<std::decay_t<O>> && (!nothrow_forward_range<O>)
             && constructible_from<iter_value_t<std::decay_t<O>>, iter_reference_t<ranges::iterator_t<IRng>>>
    uninitialized_copy_result<ranges::safe_iterator_t<IRng>, std::decay_t<O>>
    operator()(IRng&& irng, O&& ofirst) const
    {
        return uninitialized_copy_fn::impl3(begin(irng), end(irng), std::forward<O>(ofirst));
    }

  private:
    friend struct uninitialized_copy_n_fn;

    template <typename I, typename S, typename O, typename S2>
    static uninitialized_copy_result<I, O> impl4(I ifirst, S ilast, O ofirst, S2 olast)
    {
        O oit = ofirst;
        for (; ifirst != ilast && oit != olast; ++ifirst, (void) ++oit) {
            new (detail::voidify(*oit)) std::remove_reference_t<iter_reference_t<O>>(*ifirst);
        }
        return {std::move(ifirst), std::move(oit)};
    }

    template <typename I, typename S, typename O>
    static uninitialized_copy_result<I, O>
    impl3(I ifirst, S ilast, O ofirst)
    {
        O oit = ofirst;
        for (; ifirst != ilast; ++ifirst, (void) ++oit) {
            ::new (detail::voidify(*oit)) std::remove_reference_t<iter_reference_t<O>>(*ifirst);
        }
        return {std::move(ifirst), std::move(oit)};
    }
};

}

__DEF_NIEBLOID(detail::uninitialized_copy_fn, uninitialized_copy)

template <typename I, typename O>
using uninitialized_copy_n_result = uninitialized_copy_result<I, O>;

namespace detail {

struct uninitialized_copy_n_fn
{
    template <input_iterator I, nothrow_forward_iterator O, nothrow_sentinel_for<O> S>
    requires constructible_from<iter_value_t<O>, iter_reference_t<I>>
    uninitialized_copy_n_result<I, O> operator()(I ifirst, iter_difference_t<I> n, O ofirst, S olast) const
    {
        auto t = uninitialized_copy_fn::impl4(
                    make_counted_iterator(std::move(ifirst), n),
                    default_sentinel, std::move(ofirst), std::move(olast));
        return {std::move(t).in.base(), std::move(t).out};
    }

    template <input_iterator I, nothrow_forward_iterator O>
    requires constructible_from<iter_value_t<O>, iter_reference_t<I>>
    uninitialized_copy_n_result<I, O> operator()(I ifirst, iter_difference_t<I> n, O ofirst) const
    {
        auto t = uninitialized_copy_fn::impl3(
                make_counted_iterator(std::move(ifirst), n),
                default_sentinel, std::move(ofirst));
        return {std::move(t).in.base(), std::move(t).out};
    }
};

}

__DEF_NIEBLOID(detail::uninitialized_copy_n_fn, uninitialized_copy_n)

} // namespace std

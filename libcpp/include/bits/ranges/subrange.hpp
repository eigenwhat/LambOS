//
// Created by Martin Miralles-Cordal on 12/22/2019.
//

#pragma once

#include <bits/ranges/concepts.hpp>

namespace std::ranges {

enum class subrange_kind : bool { unsized, sized };

namespace detail {

template <typename I, typename S, bool = sized_sentinel_for<S, I>>
struct default_subrange_kind {
    static constexpr subrange_kind kind = subrange_kind::unsized;
};

template <typename I, typename S>
struct default_subrange_kind<I, S, true> {
    static constexpr subrange_kind kind = subrange_kind::sized;
};

}

namespace subrange_ {

template <typename I, typename S = I,
        subrange_kind = detail::default_subrange_kind<I, S>::kind>
class subrange;

}

using subrange_::subrange;


template<ranges::range R>
using safe_subrange_t = conditional_t<ranges::safe_range<R>,
        ranges::subrange<ranges::iterator_t<R>>, ranges::dangling>;

} // namespace std::ranges
//
// Created by Martin Miralles-Cordal on 12/24/2019.
//

#pragma once

#include <type_traits>
#include <utility>

namespace std {

template<typename T>
class reference_wrapper
{
    static T *_lvalue_ptr(T &r) noexcept { return std::addressof(r); }
    static void _lvalue_ptr(T&&) = delete;
  public:
    using type = T;

    template<typename U, typename = enable_if_t<!is_same_v<reference_wrapper, remove_cvref_t<U>>>,
    typename = decltype(reference_wrapper::_lvalue_ptr(std::declval<U>()))>
    reference_wrapper(U&& u) noexcept(noexcept(reference_wrapper::_lvalue_ptr(std::declval<U>())))
            : _data(reference_wrapper::_lvalue_ptr(std::forward<U>(u))) {}

    reference_wrapper(const reference_wrapper&) = default;
    reference_wrapper& operator=(const reference_wrapper&) = default;

    operator T&() const noexcept { return get(); }
    T& get() const noexcept { return *_data; }

//    template<typename... Args>
//    typename result_of<T&(Args&&...)>::type operator()(Args&&... args) const
//    {
//        return std::invoke(get(), std::forward<Args>(args)...);
//    }

  private:
    T *_data;
};

template<typename T>
reference_wrapper(T&) -> reference_wrapper<T>;

template<typename T>
inline reference_wrapper<T> ref(T& t) noexcept { return reference_wrapper<T>(t); }

template<typename T>
inline reference_wrapper<const T> cref(T const &t) noexcept { return reference_wrapper<const T>(t); }

template<typename T> void ref(const T&&) = delete;
template<typename T> void cref(const T&&) = delete;

template<typename T> inline reference_wrapper<T> ref(reference_wrapper<T> t) noexcept { return t; }
template<typename T> inline reference_wrapper<const T> cref(reference_wrapper<T> t) noexcept { return {t.get()}; }

} // namespace std
//
// Created by Martin Miralles-Cordal on 12/26/2019.
//

#pragma once

#include <Memory.hpp>

#include <concepts>

namespace sys {

template <typename T>
struct Function;

template <typename Ret, typename...Args>
struct Function<Ret(Args...)>
{
    template <typename T> requires (!std::same_as<std::decay_t<T>, Function>)
    Function(T &&fn) : impl_{new model_t<T>(std::forward<T>(fn))} {}

    Function(std::nullptr_t) : impl_{nullptr} {}

    Function(Function const &) = default;
    Ret operator()(Args...args) { return (*impl_)(args...); }

  private:
    struct concept_t
    {
        virtual ~concept_t() = default;
        virtual void operator()() = 0;
    };

    template <typename T> requires std::is_invocable_r<Ret, T, Args...>::value
    struct model_t : concept_t
    {
        explicit model_t(T const &impl) : impl_{impl} {}
        explicit model_t(T &&impl) : impl_{std::move(impl)} {}
        Ret operator()(Args... args) override { return impl_(args...); }
        T impl_;
    };

    ArcPtr<concept_t> impl_;
};

}
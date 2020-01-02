//
// Created by Martin Miralles-Cordal on 01/01/2020.
//

#pragma once

#include <concepts>

namespace sys {

/** Unique ownership pointer. It's std::unique_ptr, but shoddier. */
template <typename T> class UniquePtr
{
  public:
    constexpr UniquePtr(std::nullptr_t) noexcept : _ptr{nullptr} {}
    constexpr explicit UniquePtr(T *ptr = nullptr) noexcept : _ptr(ptr) {}
    constexpr UniquePtr &operator=(T *rhs) noexcept { reset(rhs); return *this; }

    template <typename U = T> constexpr explicit UniquePtr(UniquePtr<U> const &other) noexcept = delete;
    template <typename U = T> constexpr UniquePtr &operator=(UniquePtr<U> const &r) noexcept = delete;

    template <std::derived_from<T> U = T>
    constexpr UniquePtr(UniquePtr<U> &&other) noexcept : UniquePtr(other.release()) {}
    template <std::derived_from<T> U = T>
    constexpr UniquePtr &operator=(UniquePtr<U> &&r) noexcept { reset(r.release()); return *this; }

    ~UniquePtr() { if (_ptr) delete _ptr; }

    constexpr T *get() const noexcept { return _ptr; }

    constexpr T *release() noexcept
    {
        T *ptr = _ptr;
        _ptr = nullptr;
        return ptr;
    }

    constexpr void reset(T *newPtr) noexcept
    {
        if (_ptr == newPtr) { return; }
        if (_ptr) delete _ptr;
        _ptr = newPtr;
    }

    constexpr T *operator->() const noexcept { return _ptr; }
    constexpr T& operator*() const noexcept { return *_ptr; }
    constexpr explicit operator T*() const noexcept { return _ptr; }

    constexpr bool operator==(const UniquePtr &r) const noexcept { return _ptr == r._ptr; }
    constexpr bool operator!=(const UniquePtr &r) const noexcept { return !operator==(r); }
    constexpr explicit operator bool() const noexcept { return _ptr != nullptr; }

  private:
    template <typename> friend class UniquePtr;
    T *_ptr;
};

template <typename T, typename ... Args>
inline UniquePtr<T> make_unique(Args && ...args) { return UniquePtr<T>{new T(std::forward<Args>(args)...)}; }

} // namespace sys

//
// Created by Martin Miralles-Cordal on 01/01/2020.
//

#pragma once

#include <type_traits>

namespace sys {

template <typename T>
concept ReferenceCountable = requires(T &a) {
    a.retain();
    a.release();
};

template <typename T>
class RefCount
{
  public:
    constexpr RefCount(T *ptr) : _ptr{ptr} {}
    constexpr virtual ~RefCount() { delete_if_present_and_not_this(); }
    constexpr void retain() { ++_referenceCount; }
    constexpr void release() { if (--_referenceCount == 0) { delete_if_present(); } }

    [[nodiscard]] constexpr int count() const { return _referenceCount; }
    [[nodiscard]] constexpr T *ptr() const noexcept { return _ptr; }

  private:
    [[gnu::always_inline]] constexpr void delete_if_present() { if (_ptr) { _delete(); } }
    [[gnu::always_inline]] constexpr void delete_if_present_and_not_this()
    {
        if constexpr (!std::is_base_of_v<RefCount, T>) { _delete(); }
    }

    [[gnu::always_inline]] constexpr void _delete()
    {
        if constexpr (std::is_base_of_v<RefCount, T>) { // check for crtp
            delete this;
        } else {
            delete _ptr; _ptr = nullptr;
        }
    }

    T *_ptr;
    int _referenceCount = 1;
};

/**
 * Provides the functionality necessary for reference counting on the object
 * level. Needed for use with ArcPtr<T>.
 */
class IntrusiveRefCount : public RefCount<IntrusiveRefCount>
{
  public:
    IntrusiveRefCount() : RefCount(this) {}
};

} // namespace sys

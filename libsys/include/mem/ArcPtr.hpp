//
// Created by Martin Miralles-Cordal on 01/01/2020.
//

#pragma once

#include <mem/RefCount.hpp>
#include <mem/UniquePtr.hpp>
#include <util/Void.hpp>

#include <utility>

namespace sys {

/**
 * Shared ownership pointer. It's std::shared_ptr, but shoddier.
 * @tparam T The type to store.
 */
template <typename T>
class ArcPtr;

namespace arcptr_detail {

template <typename T> constexpr inline bool is_arcptr = false;
template <typename U> constexpr inline bool is_arcptr<ArcPtr<U>> = true;

} // namespace arcptr_detail

template <typename T>
concept is_arcptr = arcptr_detail::is_arcptr<std::decay_t<T>>;

class ArcRefCount
{
  public:
    constexpr ArcRefCount() noexcept : _data{nullptr} {}

    constexpr ArcRefCount(std::nullptr_t) noexcept : _data{nullptr} {}

    constexpr ArcRefCount& operator=(std::nullptr_t) noexcept
    {
        release();
        _data = nullptr;
        return *this;
    }

    template <typename T>
    constexpr explicit ArcRefCount(T *obj) noexcept : _data{new model_t<T>{obj}} {}

    constexpr ArcRefCount(ArcRefCount const& rhs) noexcept : _data{rhs._data} { retain(); }
    constexpr ArcRefCount& operator=(ArcRefCount const& rhs) noexcept
    {
        if (this == &rhs) { return *this; }

        release();
        _data = rhs._data;
        retain();
        return *this;
    }

    constexpr ArcRefCount(ArcRefCount&& rhs) noexcept : _data{rhs._data} { rhs._data = nullptr; }
    constexpr ArcRefCount& operator=(ArcRefCount&& rhs) noexcept
    {
        release();
        _data = rhs._data;
        rhs._data = nullptr;
        return *this;
    }

    constexpr ~ArcRefCount() { release(); }

    constexpr void retain() noexcept { if (_data) { _data->retain(); } }

    constexpr void release() noexcept
    {
        if (_data) {
            _data->release();
            if (_data->count() <= 0) {
                delete _data;
                _data = nullptr;
            }
        }
    }

    template <typename T>
    constexpr void reset(T *obj) noexcept
    {
        release();
        if (obj) {
            _data = new model_t<T>{obj};
        }
        else {
            _data = nullptr;
        }
    }

    constexpr void *ptr() const noexcept { return _data ? _data->ptr() : nullptr; }

    constexpr explicit operator bool() const noexcept { return _data; }

    constexpr bool operator==(ArcRefCount const& rhs) const noexcept = default;
    constexpr bool operator==(std::nullptr_t) const noexcept { return _data == nullptr; }

  private:
    struct concept_t
    {
        constexpr virtual ~concept_t() = default;
        constexpr virtual void retain() noexcept = 0;
        constexpr virtual void release() noexcept = 0;

        [[nodiscard]] constexpr virtual int count() const noexcept = 0;
        [[nodiscard]] constexpr virtual void *ptr() const noexcept = 0;
    };

    template <typename U>
    struct model_t : concept_t
    {
        RefCount<U> impl;

        constexpr model_t(U *obj) noexcept : impl{obj} {}
        constexpr void retain() noexcept override { return impl.retain(); }
        constexpr void release() noexcept override { return impl.release(); }

        [[nodiscard]] constexpr int count() const noexcept override { return impl.count(); }
        [[nodiscard]] constexpr void *ptr() const noexcept override { return impl.ptr(); }
    };

    concept_t *_data;
};

/**
 * Shared ownership pointer. It's std::shared_ptr, but shoddier.
 * @tparam T The type to store.
 */
template <typename T>
class ArcPtr
{
  public:
    using ElementType = T;

    constexpr ArcPtr() noexcept = default;
    constexpr ArcPtr(std::nullptr_t) noexcept {}

    template <std::derived_from<T> U>
    constexpr explicit ArcPtr(U *ptr) noexcept : _refCount{ptr} {}

    template <std::derived_from<T> U>
    constexpr ArcPtr& operator=(U *ptr) noexcept
    {
        reset(ptr);
        return *this;
    }

    constexpr ArcPtr(ArcPtr const& rhs) noexcept : ArcPtr(rhs, Nothing) {}
    constexpr ArcPtr& operator=(ArcPtr const& r) noexcept { return Assign(r); }

    template <typename U>
    constexpr ArcPtr(ArcPtr<U> const& rhs) noexcept : ArcPtr(rhs, Nothing) {}
    template <typename U>
    constexpr ArcPtr& operator=(ArcPtr<U> const& r) noexcept { return Assign(r); }

    constexpr ArcPtr(ArcPtr&& rhs) noexcept : ArcPtr(std::move(rhs), Nothing) {}
    constexpr ArcPtr& operator=(ArcPtr&& r) noexcept { return Assign(std::move(r)); }

    template <typename U>
    constexpr ArcPtr(ArcPtr<U>&& rhs) noexcept : ArcPtr(std::move(rhs), Nothing) {}
    template <typename U>
    constexpr ArcPtr& operator=(ArcPtr<U>&& r) noexcept { return Assign(std::move(r)); }

    constexpr ~ArcPtr() { reset(nullptr); }

    constexpr T *get() const noexcept { return reinterpret_cast<T *>(_refCount.ptr()); }
    constexpr T *operator->() const noexcept { return get(); }
    constexpr T& operator*() const noexcept { return *get(); }
    constexpr operator T*() const noexcept { return get(); }

    constexpr void reset(T *newPtr) noexcept { _refCount.reset(newPtr); }

    constexpr bool operator==(std::nullptr_t) const noexcept { return get() == nullptr; }

    template <typename U>
    constexpr bool operator==(ArcPtr<U> const& r) const noexcept
    {
        return get() == r.get() && _refCount == r._refCount;
    }

    template <typename U>
    constexpr bool operator==(U *r) const noexcept { return get() == r; }

    constexpr explicit operator bool() const noexcept { return get() != nullptr; }

private:
    template <typename T1>
    friend class ArcPtr;

    /** Private delegating constructor. */
    template <is_arcptr U>
    [[gnu::always_inline]]
    constexpr ArcPtr(U&& rhs, Void) noexcept : _refCount{std::forward<U>(rhs)._refCount} {}

    /** Private delegating assignment. */
    template <is_arcptr U>
    [[gnu::always_inline]]
    constexpr ArcPtr& Assign(U&& r) noexcept
    {
        if (*this != r) {
            _refCount = std::forward<U>(r)._refCount;
        }
        return *this;
    }

    ArcRefCount _refCount;
};

template <typename T, typename... Args>
constexpr ArcPtr<T> make_arc(Args&&...args) { return ArcPtr<T>{new T(std::forward<Args>(args)...)}; }

template <typename T, typename... Args>
constexpr ArcPtr<T> New(Args&&...args) { return make_arc<T>(args...); }

} // namespace sys

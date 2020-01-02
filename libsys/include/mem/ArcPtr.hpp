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
template <typename T> class ArcPtr;

namespace arcptr_detail {

template <typename T> constexpr inline bool is_arcptr = false;
template <typename U> constexpr inline bool is_arcptr<ArcPtr<U>> = true;

} // namespace arcptr_detail

template <typename T> concept is_arcptr = arcptr_detail::is_arcptr<std::decay_t<T>>;

struct ArcRefCount
{
  public:
    ArcRefCount() noexcept : _data{nullptr} {}

    ArcRefCount(std::nullptr_t) noexcept : _data{nullptr} {}
    ArcRefCount &operator=(std::nullptr_t) noexcept
    {
        release();
        _data = nullptr;
        return *this;
    }

    template <typename T>
    explicit ArcRefCount(T *obj) noexcept : _data{new model_t<T>{obj}} {}

    ArcRefCount(ArcRefCount const &rhs) noexcept : _data{rhs._data} { retain(); }
    ArcRefCount &operator=(ArcRefCount const &rhs) noexcept
    {
        release();
        _data = rhs._data;
        retain();
        return *this;
    }

    ArcRefCount(ArcRefCount &&rhs) noexcept : _data{rhs._data} { rhs._data = nullptr; }
    ArcRefCount &operator=(ArcRefCount &&rhs) noexcept
    {
        release();
        _data = rhs._data;
        rhs._data = nullptr;
        return *this;
    }

    ~ArcRefCount() { release(); }

    void retain() { if (_data) { _data->retain(); } }
    void release()
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
    void reset(T *obj) noexcept
    {
        release();
        if (obj) {
            _data = new model_t<T>{obj};
        }
        else {
            _data = nullptr;
        }
    }

    explicit operator bool() const { return _data; }

    constexpr bool operator==(ArcRefCount const &rhs) const noexcept { return _data == rhs._data; }
    constexpr bool operator==(std::nullptr_t) noexcept { return _data == nullptr; }

  private:
    struct concept_t
    {
        virtual ~concept_t() = default;
        virtual void retain() = 0;
        virtual void release() = 0;
        virtual int count() const = 0;
    };

    template <typename U>
    struct model_t : concept_t
    {
        model_t(U *obj) : _impl{obj} {}
        void retain() override { return _impl.retain(); }
        void release() override { return _impl.release(); }
        int count() const override { return _impl.count(); }
        RefCount<U> _impl;
    };

    concept_t *_data;
};

/**
 * Shared ownership pointer. It's std::shared_ptr, but shoddier.
 * @tparam T The type to store.
 */
template <typename T> class ArcPtr
{
  public:
    using ElementType = T;

    ArcPtr() : _ptr{nullptr}, _refCount{} {}
    ArcPtr(std::nullptr_t) : _ptr{nullptr}, _refCount{} {}

    template <std::derived_from<T> U>
    explicit ArcPtr(U *ptr) noexcept : _ptr{ptr}, _refCount{ptr} {}
    template <std::derived_from<T> U>
    ArcPtr &operator=(U *ptr) noexcept { reset(ptr); return *this; }

    ArcPtr(ArcPtr const &rhs) noexcept : ArcPtr(rhs, Nothing) {}
    ArcPtr &operator=(ArcPtr const &r) noexcept { return Assign(r); }
    template <typename U> ArcPtr(ArcPtr<U> const &rhs) noexcept : ArcPtr(rhs, Nothing) {}
    template <typename U> ArcPtr &operator=(ArcPtr<U> const &r) noexcept { return Assign(r); }

    ArcPtr(ArcPtr &&rhs) noexcept : ArcPtr(std::move(rhs), Nothing) {}
    ArcPtr &operator=(ArcPtr &&r) noexcept { return Assign(std::move(r)); }
    template <typename U> ArcPtr(ArcPtr<U> &&rhs) noexcept : ArcPtr(std::move(rhs), Nothing) {}
    template <typename U> ArcPtr &operator=(ArcPtr<U> &&r) noexcept { return Assign(std::move(r)); }

    ~ArcPtr() { reset(nullptr); }

    T *get() const noexcept { return _ptr; }
    T *operator->() const noexcept { return get(); }
    T& operator*() const noexcept { return *get(); }
    constexpr operator T*() const noexcept { return get(); }

    void reset(T *newPtr) noexcept
    {
        _ptr = newPtr;
        _refCount.reset(newPtr);
    }

    constexpr bool operator==(std::nullptr_t) const noexcept { return _ptr == nullptr; }
    template <typename U>
    constexpr bool operator==(const ArcPtr<U> &r) const noexcept { return _ptr == r._ptr && _refCount == r._refCount; }
    template <typename U> constexpr bool operator==(U *r) const noexcept { return get() == r; }

    explicit constexpr operator bool() const noexcept { return _ptr != nullptr; }

  private:
    template <typename T1> friend class ArcPtr;

    /** Private delegating constructor. */
    template <is_arcptr U>
    [[gnu::always_inline]] ArcPtr(U &&rhs, Void) noexcept
            : _ptr{rhs._ptr}, _refCount{std::forward<U>(rhs)._refCount}
    {
        if constexpr (std::is_rvalue_reference_v<decltype(rhs)>) { rhs._ptr = nullptr; }
    }

    /** Private delegating assignment. */
    template <is_arcptr U>
    [[gnu::always_inline]] ArcPtr &Assign(U &&r) noexcept
    {
        if (*this != r) {
            _ptr = std::forward<U>(r)._ptr;
            _refCount = std::forward<U>(r)._refCount;
        }
        return *this;
    }

    T *_ptr;
    ArcRefCount _refCount;
};

template <typename T, typename ... Args>
inline ArcPtr<T> make_arc(Args && ...args) { return ArcPtr<T>{new T(std::forward<Args>(args)...)}; }

template <typename T, typename... Args>
inline ArcPtr<T> New(Args &&...args) { return make_arc<T>(args...); }

} // namespace sys

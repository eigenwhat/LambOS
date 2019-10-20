#pragma once

#include <util/Void.hpp>

#include <utility>

template <typename T>
concept ReferenceCountable = requires(T a) {
    { a.retain() };
    { a.release() };
};


template <typename T>
class RefCount
{
  public:
    RefCount(T *ptr) : _ptr{ptr} {}
    virtual ~RefCount() { delete_if_present_and_not_this(); }
    void retain() { ++_referenceCount; }
    void release() { if (--_referenceCount == 0) { delete_if_present(); } }

  private:
    [[gnu::always_inline]] void delete_if_present()
    {
        if (_ptr) {
            if constexpr (std::is_base_of_v<RefCount, T>) { // check for crtp
                delete this;
            }else {
                delete _ptr; _ptr = nullptr;
            }
        }
    }
    [[gnu::always_inline]] void delete_if_present_and_not_this()
    {
        if (_ptr && !std::is_base_of_v<RefCount, T>) {
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


template <typename T, bool = ReferenceCountable<T>> class ArcPtr;

/**
 * Fucky version of shared_ptr which only works on ReferenceCountable objects.
 * I'd SFINAE but lol no stdlib yet.
 */
template <typename T> class ArcPtr<T, true>
{
  public:
    using ElementType = T;
    ArcPtr() noexcept : _ptr{nullptr} {}

    ArcPtr(std::nullptr_t) noexcept : _ptr{nullptr} {}

    ArcPtr(std::nullptr_t, bool) noexcept = delete;

    template <typename U = T>
    explicit ArcPtr(U *ptr, bool initialRetain = true) noexcept : _ptr(ptr)
    {
        if (ptr && initialRetain) ptr->retain();
    }

    ArcPtr(const ArcPtr &rhs) noexcept : ArcPtr(rhs, Nothing) {}
    template <typename U> ArcPtr(const ArcPtr<U> &rhs) noexcept : ArcPtr(rhs, Nothing) {}

    ArcPtr(ArcPtr &&rhs) noexcept : ArcPtr(rhs, Nothing) {}
    template <typename U> ArcPtr(ArcPtr<U> &&rhs) noexcept : ArcPtr(std::move(rhs), Nothing) {}

    ~ArcPtr() { reset(nullptr); }

    T *get() const noexcept { return _ptr; }

    void reset(T *newPtr) noexcept
    {
        if (_ptr) _ptr->release();
        _ptr = newPtr;
        if (_ptr) _ptr->retain();
    }

    T *operator->() const noexcept { return _ptr; }
    T& operator*() const noexcept { return *_ptr; }

    ArcPtr &operator=(ArcPtr const &r) noexcept { return Assign(r); }
    template <typename U> ArcPtr &operator=(ArcPtr<U> const &r) noexcept { return Assign(r); }

    ArcPtr &operator=(ArcPtr &&r) noexcept { return Assign(std::move(r)); }
    template <typename U> ArcPtr &operator=(ArcPtr<U> &&r) noexcept { return Assign(std::move(r)); }

    template <typename U>
    ArcPtr &operator=(U *rhs) noexcept { if (*this != rhs) { reset(rhs); } return *this; }

    template <typename U>
    constexpr bool operator==(const ArcPtr<U> &r) const noexcept { return _ptr == r._ptr; }
    template <typename U>
    constexpr bool operator!=(const ArcPtr<U> &r) const noexcept { return !operator==(r); }

    template <typename U>
    constexpr bool operator==(U *r) const noexcept { return _ptr == r; }
    template <typename U>
    constexpr bool operator!=(U *r) const noexcept { return !operator==(r); }

    explicit constexpr operator bool() const noexcept { return _ptr != nullptr; }

  private:
    /** Private delegating constructors. */
    /**  @{ */
    template <typename U>
    [[gnu::always_inline]]
    ArcPtr(const ArcPtr<U> &rhs, Void) noexcept : ArcPtr(rhs.get(), true) {}

    template <typename U>
    [[gnu::always_inline]]
    ArcPtr(ArcPtr<U> &&rhs, Void) noexcept : ArcPtr{rhs.get(), false} { rhs._ptr = nullptr; }
    /**  @} */

    /** Private delegating assignment. */
    /**  @{ */
    template <typename U>
    [[gnu::always_inline]]
    ArcPtr &Assign(ArcPtr<U> const &r) noexcept { if (*this != r) { reset(r.get()); } return *this; }

    template <typename U>
    [[gnu::always_inline]]
    ArcPtr &Assign(ArcPtr<U> &&r) noexcept
    {
        if (*this != r) {
            if (_ptr) _ptr->release();
            _ptr = r._ptr;
            r._ptr = nullptr;
        }
        return *this;
    }
    /** @} */

    template <typename, bool> friend class ArcPtr;
    T *_ptr;
};

/**
 * Creates a new T on the heap, then constructs an ArcPtr<T> with it, then
 * finally releases, making the ArcPtr the sole retainer.
 * @tparam Args The types for one of T's constructors. (deducible)
 * @param args The arguments for one of T's constructors.
 * @return The ArcPtr.
 */
template <ReferenceCountable T, typename ... Args>
static ArcPtr<T> make_arc(Args && ...args)
{
    ArcPtr<T> ptr(new T(std::forward<Args>(args)...), false);
    return ptr;
}

template <typename T>
ArcPtr<T> as_arc(T *ptr, bool initialRetain = true)
{
    return ArcPtr<T>(ptr, initialRetain);
}

/** RAII object releaser. */
template <typename T> requires ReferenceCountable<T>
class Autorelease : public ArcPtr<T>
{
  public:
    Autorelease(T *ptr, bool retain = false) : ArcPtr<T>(ptr, retain) {}
    operator T*() const { return ArcPtr<T>::get(); }
};

/**
 * Handy autorelease macro that almost looks like a language construct.
 * Releases the object when exiting the enclosing scope.
 */
#define autorelease(object) auto autorel__COUNTER__ = Autorelease((object))

/**
 * Macro which creates a scope where the object is first retained at the
 * beginning of the scope and then gets released at the end of scope.
 *
 * Like autorelease but with an initial retain.
 */
#define autoretain(object) if (auto autorel__COUNTER__ = Autorelease((object), true); true)

/** Unique ownership pointer. It's std::unique_ptr, but shoddier. */
template <typename T> class UniquePtr
{
  public:
    constexpr explicit UniquePtr(std::nullptr_t) noexcept : _ptr{nullptr} {}
    constexpr explicit UniquePtr(T *ptr = nullptr) noexcept : _ptr(ptr) {}
    constexpr UniquePtr &operator=(T *rhs) noexcept { reset(rhs); return *this; }

    template <typename U = T> constexpr explicit UniquePtr(UniquePtr<U> const &other) noexcept = delete;
    template <typename U = T> constexpr UniquePtr &operator=(UniquePtr<U> const &r) noexcept = delete;

    template <typename U = T>
    constexpr explicit UniquePtr(UniquePtr<U> &&other) noexcept : UniquePtr(other.release()) {}
    template <typename U = T>
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
static UniquePtr<T> make_unique(Args && ...args) { return UniquePtr<T>{new T(std::forward<Args>(args)...)}; }


/**
 * Marshals a non-ReferenceCountable type into the reference counting system.
 * @tparam T The type to store.
 */
template <typename T> class ArcWrapper
{
  public:
    ArcWrapper(T *obj) : _ptr(obj), _refCount{make_unique<RefCount<T>>(obj)} {}

    T *operator->() const noexcept { return _ptr; }
    T& operator*() const noexcept { return *_ptr; }
    T *get() const noexcept { return _ptr; }
    void retain() { _refCount->retain(); }
    void release() { _refCount->release(); }
  private:
    T *_ptr;
    UniquePtr<RefCount<T>> _refCount;
};

/**
 * Shared ownership pointer. It's std::shared_ptr, but shoddier.
 * @tparam T The type to store.
 */
template <typename T> class ArcPtr<T, false>
{
  public:
    using ElementType = T;
    ArcPtr() noexcept : _data{nullptr} {}

    ArcPtr(std::nullptr_t) noexcept : _data{nullptr} {}

    template <typename U> explicit ArcPtr(U *ptr) noexcept : _data{} { reset(ptr); }
    template <typename U> ArcPtr &operator=(U *ptr) noexcept { if (*this != ptr) { reset(ptr); } return *this; }

    ArcPtr(const ArcPtr &rhs) noexcept : ArcPtr(rhs, Nothing) {}
    template <typename U> ArcPtr(const ArcPtr<U> &rhs) noexcept : ArcPtr(rhs, Nothing) {}

    ArcPtr(ArcPtr &&rhs) noexcept : ArcPtr(rhs, Nothing) {}
    template <typename U> ArcPtr(ArcPtr<U> &&rhs) noexcept : ArcPtr(std::move(rhs), Nothing) {}

    ~ArcPtr() { reset(nullptr); }

    T *get() const noexcept { return _data ? _data->get() : nullptr; }

    void reset(T *newPtr) noexcept
    {
        if (_data) _data->release();
        if (newPtr) { _data = new ArcWrapper<T>{newPtr}; } else { _data = nullptr; }
    }

    T *operator->() const noexcept { return get(); }
    T& operator*() const noexcept { return *get(); }
    constexpr operator T*() const noexcept { return get(); }

    ArcPtr &operator=(ArcPtr const &r) noexcept { return Assign(r); }
    template <typename U> ArcPtr &operator=(ArcPtr<U> const &r) noexcept { return Assign(r); }

    ArcPtr &operator=(ArcPtr &&r) noexcept { return Assign(std::move(r)); }
    template <typename U> ArcPtr &operator=(ArcPtr<U> &&r) noexcept { return Assign(std::move(r)); }

    template <typename U> constexpr bool operator==(const ArcPtr<U> &r) const noexcept { return _data == r._data; }
    template <typename U> constexpr bool operator!=(const ArcPtr<U> &r) const noexcept { return !operator==(r); }

    template <typename U> constexpr bool operator==(U *r) const noexcept { return get() == r; }
    template <typename U> constexpr bool operator!=(U *r) const noexcept { return !operator==(r); }

    explicit constexpr operator bool() const noexcept { return _data != nullptr; }

  private:
    /** Private delegating constructors. */
    /**  @{ */
    template <typename U>
    [[gnu::always_inline]]
    ArcPtr(const ArcPtr<U> &rhs, Void) noexcept : _data{rhs._data} { if (_data) { _data->retain(); } }

    template <typename U>
    [[gnu::always_inline]]
    ArcPtr(ArcPtr<U> &&rhs, Void) noexcept : _data{rhs._data} { rhs._ptr = nullptr; }
    /**  @} */

    /** Private delegating assignment. */
    /**  @{ */
    template <typename U>
    [[gnu::always_inline]]
    ArcPtr &Assign(ArcPtr<U> const &r) noexcept
    {
        if (*this != r) {
            if (_data) { _data->release(); }
            _data = r._data;
            if (_data) { _data->retain(); }
        }
        return *this;
    }

    template <typename U>
    [[gnu::always_inline]]
    ArcPtr &Assign(ArcPtr<U> &&r) noexcept
    {
        if (*this != r) {
            if (_data) { _data->release(); }
            _data = r._data;
            r._data = nullptr;
        }
        return *this;
    }
    /** @} */

    ArcWrapper<T> *_data;
};

template <typename T, typename ... Args> requires !ReferenceCountable<T>
static ArcPtr<T> make_arc(Args && ...args) { return ArcPtr<T>{new T(std::forward<Args>(args)...)}; }

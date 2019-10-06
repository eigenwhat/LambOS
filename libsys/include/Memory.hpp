#pragma once

#include <utility>

template <typename T>
concept ReferenceCountable = requires(T a) {
    { a.retain() };
    { a.release() };
};

/**
 * Provides the functionality necessary for reference counting on the object
 * level. Needed for use with ArcPtr<T>.
 */
class IntrusiveRefCount
{
  public:
    virtual ~IntrusiveRefCount() {}
    void retain() { ++_referenceCount; }
    void release()
    {
        if (--_referenceCount == 0) {
            delete this;
        }
    }

  private:
    int _referenceCount = 1;
};

/**
 * Marshals a non-ReferenceCountable type into the reference counting system.
 * @tparam T The type to store.
 */
template <typename T> class ArcWrapper : public IntrusiveRefCount
{
  public:
    /**
     * Creates a new T on the heap, then constructs an ArcWrapper<T> with it.
     * @tparam Args The types for one of T's constructors. (deducible)
     * @param args The arguments for one of T's constructors.
     * @return The ArcWrapper.
     */
    template <typename ... Args>
    static ArcWrapper<T> make(Args && ...args)
    {
        ArcWrapper<T> ptr(new T(args...));
        return ptr;
    }

    ArcWrapper(T *obj) : _obj(obj) {}
    ~ArcWrapper() { delete _obj; }

    T& operator*() { return *_obj; }
    T const & operator*() const { return *_obj; }
    T* operator->() { return _obj; }
    T const * operator->() const { return _obj; }

  private:
    T *_obj;
};

/**
 * Fucky version of shared_ptr which only works on ReferenceCountable objects.
 * I'd SFINAE but lol no stdlib yet.
 */
template <typename T> class ArcPtr
{
  public:
    ArcPtr(std::nullptr_t) noexcept : _ptr{nullptr} {}

    explicit ArcPtr(T *ptr = nullptr, bool initialRetain = true) noexcept : _ptr(ptr)
    {
        if (ptr && initialRetain) ptr->retain();
    }

    ArcPtr(const ArcPtr &other) noexcept : ArcPtr(other.get()) {}

    template <typename U>
    explicit ArcPtr(const ArcPtr<U> &other) noexcept : ArcPtr(other.get()) {}

    ArcPtr(ArcPtr &&other) noexcept = default;

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

    ArcPtr &operator=(ArcPtr const &r) noexcept
    {
        reset(r.get());
        return *this;
    }

    template <typename U>
    ArcPtr &operator=(ArcPtr<U> const &r) noexcept
    {
        reset(r.get());
        return *this;
    }

    ArcPtr &operator=(ArcPtr &&r) noexcept
    {
        if (_ptr) _ptr->release();
        _ptr = r._ptr;
        r._ptr = nullptr;
        return *this;
    }

    template <typename U>
    ArcPtr &operator=(ArcPtr<U> &&r) noexcept
    {
        if (_ptr) _ptr->release();
        _ptr = r._ptr;
        r._ptr = nullptr;
        return *this;
    }

    ArcPtr &operator=(T *rhs) noexcept
    {
        reset(rhs);
        return *this;
    }

    bool operator==(const ArcPtr &r) const noexcept { return _ptr == r._ptr; }

    bool operator!=(const ArcPtr &r) const noexcept { return !operator==(r); }

    explicit operator bool() const noexcept { return _ptr != nullptr; }

  private:
    template <typename> friend class ArcPtr;
    T *_ptr;
};

/**
 * Creates a new T on the heap, then constructs an ArcPtr<T> with it, then
 * finally releases, making the ArcPtr the sole retainer.
 * @tparam Args The types for one of T's constructors. (deducible)
 * @param args The arguments for one of T's constructors.
 * @return The ArcPtr.
 */
template <typename T, typename ... Args>
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
template <typename T>
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

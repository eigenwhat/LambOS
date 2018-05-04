#pragma once

/**
 * Provides the functionality necessary for reference counting on the object
 * level. Needed for use with ArcPtr<T>.
 */
class ReferenceCountable
{
  public:
    virtual ~ReferenceCountable() {}
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
template <typename T> class ArcWrapper : public virtual ReferenceCountable
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
    /**
     * Creates a new T on the heap, then constructs an ArcPtr<T> with it, then
     * finally releases, making the ArcPtr the sole retainer.
     * @tparam Args The types for one of T's constructors. (deducible)
     * @param args The arguments for one of T's constructors.
     * @return The ArcPtr.
     */
    template <typename ... Args>
    static ArcPtr<T> make(Args && ...args)
    {
        ArcPtr<T> ptr(new T(args...), false);
        return ptr;
    }

    ArcPtr(T *ptr = nullptr, bool initialRetain = true) : _ptr(ptr)
    {
        if (ptr && initialRetain) ptr->retain();
    }

    ArcPtr(const ArcPtr &other) : ArcPtr(other.get()) {}

    template <typename U>
    ArcPtr(const ArcPtr<U> &other) : ArcPtr(other.get()) {}

    ArcPtr(ArcPtr &&other) = default;

    ~ArcPtr() { reset(nullptr); }

    T *get() const { return _ptr; }

    void reset(T *newPtr)
    {
        if (_ptr) _ptr->release();
        _ptr = newPtr;
        if (_ptr) _ptr->retain();
    }

    T *operator->() const { return _ptr; }

    T& operator*() const { return *_ptr; }

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

    bool operator==(const ArcPtr &r) const { return _ptr == r._ptr; }

    bool operator!=(const ArcPtr &r) const { return !operator==(r); }

    explicit operator bool() const { return _ptr != nullptr; }

  private:
    template <typename> friend class ArcPtr;
    T *_ptr;
};

template <typename T>
ArcPtr<T> AsArcPtr(T *ptr, bool initialRetain = true)
{
    return ArcPtr<T>(ptr, initialRetain);
}

/** RAII object releaser. */
template <typename T>
ArcPtr<T> Autorelease(T *ptr) { return ArcPtr<T>(ptr, false); }

/** Handy autorelease macro that almost looks like a language construct. */
#define autorelease(object) auto autorel__COUNTER__ = Autorelease((object))

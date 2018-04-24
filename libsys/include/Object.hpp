#pragma once

#include <stddef.h>
#include <stdint.h>

class Object
{
public:
    Object() : _referenceCount(1) {}
    virtual ~Object() {}
    void retain() const { ++_referenceCount; }
    void release() const
    {
        if (--_referenceCount == 0) {
            delete (Object *) this;
        }
    }

private:
    mutable int _referenceCount;
};

/**
 * Fucky version of shared_ptr, except it only works on objects of Object type.
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

    ArcPtr(ArcPtr &&other) = default;

    ~ArcPtr() { reset(nullptr); }

    T *get() const { return _ptr; }

    void reset(T *newPtr) {
        if (_ptr) _ptr->release();
        _ptr = newPtr;
        if (_ptr) _ptr->retain();
    }

    T *operator->() const { return _ptr; }

    ArcPtr &operator=(ArcPtr const &r) noexcept
    {
        reset(r.get());
        return *this;
    }

    ArcPtr &operator=(ArcPtr &&r) noexcept {
        if (_ptr) _ptr->release();
        _ptr = r._ptr;
        r._ptr = nullptr;
        return *this;
    }

  private:
    T *_ptr;
};

/** RAII object releaser. */
class Autoreleaser
{
  public:
    Autoreleaser(Object *ptr) : _ptr(ptr) {}
    ~Autoreleaser() { if (_ptr) _ptr->release(); }
  private:
    Object *_ptr;
};

/** Handy autorelease macro that almost looks like a language construct. */
#define autorelease(object) Autoreleaser autorelease__COUNTER__((object));

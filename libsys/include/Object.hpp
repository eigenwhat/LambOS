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

template <typename T> class Array : public Object
{
  public:
    Array(size_t size) : _array(new T[size]) {}
    ~Array() { delete[] _array; }

    T *get() { return _array; }
    const T *get() const { return _array; }
    operator const T*() const { return get(); }
    operator T*() { return get(); }

    T & operator[](size_t idx) { return _array[idx]; }
    T const & operator[](size_t idx) const { return _array[idx]; }

  private:
    T *_array;
};

/**
 * Fucky version of shared_ptr, except it only works on objects of Object type.
 * I'd SFINAE but lol no stdlib yet.
 */
template <typename T> class ArcPtr
{
  public:
    ArcPtr(T *ptr = nullptr) : _ptr(ptr) { if (ptr) ptr->retain(); }

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

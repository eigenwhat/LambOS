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
        --_referenceCount;

        if (_referenceCount == 0) {
            delete (Object *) this;
        }
    }

    virtual bool isEqualTo(Object *o) { return this == o; }

private:
    mutable int _referenceCount;
};

/** Fucky version of shared_ptr, except it only works on objects of Object type. I'd SFINAE but lol no stdlib yet. */
template <typename T> class ArcPtr {
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

    ArcPtr &operator=(const ArcPtr &r) noexcept
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

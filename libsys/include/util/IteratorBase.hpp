#pragma once

#include <Object.hpp>

/**
 * CRTP using base for all Iterators. This allows us to return by value a
 * derived Iterator object. This Iterator can then use the pimpl idiom to
 * leverage runtime polymorphism without being forced to use pointers/references
 * in its public interface.
 * @tparam Derived The public facing Iterator template.
 * @tparam T The element type for the Iterator's collection.
 */
template <typename Derived, typename T>
class IteratorBase : public Object
{
  public:
    /**
     * (pre-increment) Advances the iterator one element forward.
     * @return A reference to the next Iterator in the Collection.
     */
    virtual Derived& operator++() = 0;

    /**
     * (post-increment) Advances the Iterator one element forward.
     * @return An Iterator equal to this before the increment.
     */
    virtual Derived operator++(int) = 0;

    /**
     * Retrieves a reference to the pointed-to element.
     * @return A reference to the element the Iterator is pointing at. If it
     *         isn't pointing to anything (e.g. end of the Collection), the
     *         result is undefined.
     */
    virtual T& operator*() const = 0;

    /**
     * Equality operator.
     * @param rhs The iterator to compare to.
     * @return `true` if they are equal, `false` otherwise.
     */
    virtual bool operator==(const Derived &rhs) const = 0;

    /**
     * Inequality operator. Equivalent to !(this == rhs).
     * @param rhs The Iterator to compare to.
     * @return `true` if they're not equal, `false` otherwise.
     */
    bool operator!=(const Derived &rhs) const { return !operator==(rhs); }

    /** Identifies the class of Impl for use in non-RTTI environments. */
    virtual size_t classId() const = 0;

    /** Returns the public Iterator instance. */
    Derived *publicInstance() const { return _publicInstance; }

  protected:
    Derived *_publicInstance;
};

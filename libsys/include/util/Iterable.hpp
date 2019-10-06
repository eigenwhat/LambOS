#pragma once

#include <util/Collection.hpp>
#include <util/IteratorBase.hpp>

template <typename T> class Iterator;

/**
 * An Iterable Collection is a Collection that supports traversal via special
 * pointer objects called {@ref Iterator}s.
 * @tparam T
 */
template <typename T, typename Tv>
concept Iterable = Collection<T> && requires(T const &a) {
    /**
     * Retrieves an Iterator set to the frontmost element of the Collection. If
     * the Collection is empty, begin() == end().
     * @return An Iterator pointing to the frontmost element of the Collection.
     */
    { a.begin() } -> Same<Iterator<Tv>>;

    /**
     * Retrieves an Iterator marking the end of the Collection. It should *NOT*
     * point to any element in the Collection, but instead act as a placeholder
     * which can be compared against to determine when iteration has completed.
     * @return An Iterator with a special value signifying the end of the
     *         Collection. The return value from dereferencing it is undefined.
     */
    { a.end() } -> Same<Iterator<Tv>>;
};

template <typename T>
class IteratorImpl : public IteratorBase<Iterator<T>, T>
{
  protected:
    friend class Iterator<T>;
    IteratorImpl *implOf(Iterator<T> const &i) const { return i._impl.get(); }
};

/**
 * An Iterator is an object which can be used to traverse the elements of a
 * Collection. It acts roughly like a pointer object.
 */
template <typename T>
class Iterator : public Object
{
  public:
    static Iterator newIterator(IteratorImpl<T> *impl) { return Iterator{impl}; }

    /**
     * (pre-increment) Advances the iterator one element forward.
     * @return A reference to the next Iterator in the Collection.
     */
    Iterator& operator++() { return _impl->operator++(); }

    /**
     * (post-increment) Advances the Iterator one element forward.
     * @return An Iterator equal to this before the increment.
     */
    Iterator operator++(int _) { return _impl->operator++(_); }

    /**
     * Retrieves a reference to the pointed-to element.
     * @return A reference to the element the Iterator is pointing at. If it
     *         isn't pointing to anything (e.g. end of the Collection), the
     *         result is undefined.
     */
    T& operator*() const { return _impl->operator*(); }

    /**
     * Equality operator.
     * @param rhs The iterator to compare to.
     * @return `true` if they are equal, `false` otherwise.
     */
    bool operator==(const Iterator &rhs) const
    {
        return _impl->operator==(rhs);
    }

    /**
     * Inequality operator. Equivalent to !(this == rhs).
     * @param rhs The Iterator to compare to.
     * @return `true` if they're not equal, `false` otherwise.
     */
    bool operator!=(const Iterator &rhs) const { return !operator==(rhs); }

    /**
     * Used to identify what class of Iterator this is in non-RTTI
     * environments.
     * @return Some identifier to compare against.
     */
    size_t classId() const { return _impl->classId(); }

  protected:
    friend class Iterable;
    Iterator(IteratorImpl<T> *impl) : _impl(impl)
    {
        impl->_publicInstance = this;
    }

  private:
    ArcPtr<IteratorImpl<T>> _impl;
};

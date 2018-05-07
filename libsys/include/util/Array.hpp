#pragma once

#include <util/Collection.hpp>

/**
 * C++ container for a simple fixed-width array.
 * @tparam T The type of object the Array contains.
 * @todo Decide if it's really sound designwise subclassing from Collection<T>.
 */
template <typename T> class Array : public Collection<T>
{
  public:
    Array(size_t size) : _array(new T[size]), _size(size) {}

    ~Array() { if (_array) { delete[] _array; } }

    Array(Array const &other) = delete;

    /**
     * Moves the contents of one array to the other.
     * @note This leaves the moved-from array unusable.
     * @param other The array whose contents to move.
     */
    Array(Array &&other) : _size(other.size())
    {
        _array = other._array;
        other._array = nullptr;
    }

    /**
     * Returns the backing C style array.
     * @return A T * pointing to the front of the array.
     */
    T *get() { return _array; }

    /**
     * Returns the backing C style array.
     * @return A T * pointing to the front of the array.
     */
    const T *get() const { return _array; }

    /**
     * Allows streamlined conversion to the backing T[].
     * @return A T * pointing to the front of the array.
     */
    operator const T*() const { return get(); }

    /**
     * Allows streamlined conversion to the backing T[].
     * @return A T * pointing to the front of the array.
     */
    operator T*() { return get(); }

    /**
     * Provides direct access to the objects in the backing array using the
     * natural array subscript syntax.
     * @return The object in the backing array at the given index.
     */
    T & operator[](size_t idx) { return _array[idx]; }

    /**
     * Provides direct access to the objects in the backing array using the
     * natural array subscript syntax.
     * @return The object in the backing array at the given index.
     */
    T const & operator[](size_t idx) const { return _array[idx]; }

    /**
     * A fixed width container is never empty.
     * @return `false`, because it's not empty.
     */
    bool isEmpty() const override { return false; }

    /**
     * The number of elements in the Array. As a fixed-width container, this is
     * always equal to its initial capacity.
     * @return A size_t equal to the number of elements.
     */
    size_t size() const override { return _size; }

    /**
     * A fixed-width Array cannot grow by definition, so insertion would require
     * overwriting a pre-existing slot, and selection of an element to replace
     * is well outside the scope of this class. Use the subscript operator
     * (this[]) to modify the object at that index instead.
     * @return `false`, as the Collection remains unchanged.
     */
    bool insert(const T&) override { return false; }

    /**
     * Removing an object from a fixed-width container is inconsequential, as
     * the space will still be occupied. Use the subscript operator (this[]) to
     * modify the object at that index instead.
     * @return `false`.
     */
    bool remove(const T&) override { return false; }

  private:
    T *_array;
    const size_t _size;
};

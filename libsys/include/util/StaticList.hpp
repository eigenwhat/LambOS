//
// Created by Martin Miralles-Cordal on 4/23/2018.
//

#pragma once

#include <util/Collection.hpp>

namespace sys {

/**
 * C++ container for a simple fixed-width array.
 * @tparam T The type of object the Array contains.
 */
template <typename T> class StaticList
{
  public:
    using value_type = T;

    StaticList(size_t size) : _array(nullptr), _size(size)
    {
        if (size) { _array = new T[size]; }
    }

    ~StaticList() { if (_array) { delete[] _array; } }

    StaticList(StaticList const &other) = delete;

    /**
     * Moves the contents of one array to the other.
     * @note This leaves the moved-from array unusable.
     * @param other The array whose contents to move.
     */
    StaticList(StaticList &&other) : _array(other._array), _size(other.size())
    {
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
    T const *get() const { return _array; }

    /**
     * Allows streamlined conversion to the backing T[].
     * @return A T * pointing to the front of the array.
     */
    operator T const *() const { return get(); }

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
    bool isEmpty() const { return false; }

    /**
     * The number of elements in the Array. As a fixed-width container, this is
     * always equal to its initial capacity.
     * @return A size_t equal to the number of elements.
     */
    size_t size() const { return _size; }

    /**
     * Destroys the array, then reallocates a fresh one of the same size.
     * @return `true`.
     */
    bool clear()
    {
        if (_array) { delete[] _array; }
        if (_size) { _array = new T[_size]; }
        return true;
    }

    StaticList &operator=(StaticList &&rhs)
    {
        auto &size = const_cast<size_t&>(_size);
        size = rhs._size;
        if (_array) { delete[] _array; }
        _array = rhs._array;
        rhs._array = nullptr;
        return *this;
    }

  private:
    T *_array;
    const size_t _size;
};

ASSERT_IS_COLLECTION(StaticList)

} // libsys namespace

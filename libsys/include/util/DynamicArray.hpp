#pragma once

#include <Object.hpp>

#include <utility>
#include <cstddef>
#include <stdint.h>

template <typename T>
class DefaultResizer
{
  public:
    /**
     * Creates a new memory block, copying over the data from the old block.
     * The old block is *not* deallocated or otherwise modified, outside of any
     * side effects caused by the element type's assignment operator.
     * @param oldData The data to resize.
     * @param oldSize The size of the old data.
     * @param newSize The size to resize to.
     * @param offset The offset from the beginning where the old data should go.
     * @return The new memory block.
     */
    static T *resize(T *oldData, size_t oldSize, size_t newSize, size_t offset = 0)
    {
        T *newData = new T[newSize];
        for (size_t i = 0; i < oldSize; ++i) {
            newData[i+offset] = std::move(oldData[i]);
        }
        return newData;
    }
};

template <typename T, typename Resizer = DefaultResizer<T>>
class DynamicArray : public Object
{
  public:
    static constexpr size_t kDefaultSize = 8;

    DynamicArray() : DynamicArray(kDefaultSize) {}
    DynamicArray(size_t reserve) : _capacity(reserve), _data(new T[_capacity]) {}

    ~DynamicArray() { delete[] _data; }

    size_t capacity() const { return _capacity; }

    /**
     * Resizes the data block by allocating a new block, then copying over all
     * the data from the old block to the new block.
     * @param newSize The new size. Defaults to (previous capacity * 2).
     * @param offset The offset from the start of the new block where the old
     *               data should be moved to. Handy when resizing due to a
     *               push() operation in order to leave room for the new element
     *               at the front. The offset should be less than or equal to
     *               (newSize - capacity), otherwise the behavior is undefined.
     *               Defaults to 0.
     */
    void resize(size_t newSize = 0, size_t offset = 0)
    {
        if (newSize == 0) newSize = nextCapacity();
        T *newData = Resizer::resize(_data, _capacity, newSize, offset);
        delete[] _data;
        _data = newData;
        _capacity = newSize;
    }

    /**
     * Shifts the data in the array.
     * This is a potentially destructive operation.
     * @param offset The amount to shift.
     * @param shiftLeft If true, shifts left. Otherwise, shift right.
     */
    void shift(size_t offset, bool shiftLeft = false)
    {
        if (shiftLeft) _moveLeft(_data, _data, _capacity, offset);
        else _moveRight(_data, _data, _capacity, offset);
    }

    /**
     * Clears the array, destroying all contents.
     * @param newSize The new size. Defaults to kDefaultSize.
     */
    void clear(size_t newSize = kDefaultSize)
    {
        delete[] _data;
        _data = new T[newSize];
        _capacity = newSize;
    }

    /**
     * Returns the backing C style array.
     * @return A T * pointing to the front of the array.
     */
    T const *get() const { return _data; }

    /**
     * Allows streamlined conversion to the backing T[].
     * @return A T * pointing to the front of the array.
     */
    operator T const *() const { return get(); }

    /**
     * Returns the element at the given index.  No bounds checking is performed.
     * @param idx The index of the object.
     * @return The object at that index. If the index is out of bounds, the
     *         return value is undefined.
     */
    T& operator[](size_t idx) const { return _data[idx]; }

  private:
    /**
     * Moves `_size` elements from one array to another.
     * @param from Array to copy from
     * @param to Array to copy to
     * @param amount The number of elements in `from`
     * @param offset where `from` goes within `to` (i.e. to[offset] = from[0])
     */
    void _moveRight(T *from, T *to, size_t amount, size_t offset)
    {
        // copy back-to-front to avoid overlap issues
        // special care needed with reverse for loops and unsigned types
        for (size_t remaining = amount; remaining > 0; --remaining) {
            size_t idx = remaining - 1;
            to[idx+offset] = std::move(from[idx]);
        }
    }

    void _moveLeft(T *from, T *to, size_t amount, size_t offset)
    {
        for (size_t i = offset; i < amount; ++i)
        {
            to[i-offset] = std::move(from[i]);
        }
    }

    size_t nextCapacity() const { return _capacity * 2; }

    size_t _capacity = 0;
    T *_data = nullptr;
};
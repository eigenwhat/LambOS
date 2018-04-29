#pragma once

#include <util/List.hpp>
#include <mem/liballoc.h>

template <typename T>
class ArrayList : public List<T>
{
  public:
    /** Constructs an ArrayList with the default pre-allocation. */
    ArrayList() : ArrayList(8) {}

    /**
     * Constructs an ArrayList, pre-allocating space for `reserve` number of
     * elements.
     * @param reserve The number of elements to pre-allocate space for.
     */
    ArrayList(size_t reserve) : _capacity(reserve), _data(new T[_capacity]) {}

    /**
     * Adds an element to the top of the ArrayList.
     * @note This is an O(n) operation! If you're doing this often, consider
     *       the ordering of your data, or use a different List implementation.
     * @param obj The object to add.
     * @return `true` if an object was added. `false` otherwise.
     */
    bool push(const T &obj) override
    {
        _shiftRight(1);
        _data[0] = obj;
        ++_size;
        return true;
    }

    /**
     * Adds an element to the back of the ArrayList.
     * @param obj The object to add.
     * @return `true` if an object was added. `false` otherwise.
     */
    bool enqueue(const T &obj) override
    {
        if (_size == _capacity) {
            _resize();
        }

        _data[_size] = obj;
        ++_size;
        return true;
    }

    /**
     * Removes an element from the top of the ArrayList.
     * @note This is an O(n) operation! If you're doing this often, consider
     *       the ordering of your data, or use a different List implementation.
     * @return The object. If the ArrayList is empty, the return value is undefined.
     */
    T pop() override
    {
        T ret = peek();
        _shiftLeft(1);
        --_size;
        return ret;
    }

    /**
     * Removes an element from the back of the ArrayList.
     * @return The object. If the ArrayList is empty, the return value is undefined.
     */
    T unqueue() override { --_size; return _data[_size]; }

    /**
     * Returns the element at the top of the ArrayList without removing it.
     * @return The object. If the ArrayList is empty, the return value is undefined.
     */
    const T &peek() const override { return _data[0]; }

    /**
     * Returns the element at the back of the ArrayList without removing it.
     * @return The object. If the ArrayList is empty, the return value is undefined.
     */
    const T &peekBack() const override { return _data[_size-1]; }

    /**
     * Returns whether or not the ArrayList is empty.
     * @return `true` if empty, `false` otherwise.
     */
    bool isEmpty() const override { return _size == 0;}

    /**
     * The number of elements in the ArrayList.
     * @return A size_t equal to the number of elements.
     */
    size_t size() const override { return _size; }

    /**
     * Adds the object to the ArrayList, growing its size.
     *
     * @param object The object to add.
     * @return `true` if the ArrayList changed. `false` otherwise.
     */
    bool insert(const T &object) override { return enqueue(object); }

    /**
     * Removes the object from the ArrayList, reducing its size.
     *
     * @param object The object to remove.
     * @return `true` if the ArrayList changed. `false` otherwise.
     */
    bool remove(const T &) override {
        return false;
    }

    /**
     * Returns the element at the given index.  No bounds checking is performed.
     * @param idx The index of the object.
     * @return The object at that index. If the index is out of bounds, the
     *         return value is undefined.
     */
    T& operator[](size_t idx) const override { return _data[idx]; }

    size_t capacity() const { return _capacity; }

  private:
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
    void _resize(size_t newSize = 0, size_t offset = 0)
    {
        if (newSize == 0) newSize = nextCapacity();
        T *oldData = _data;
        T *newData = new T[newSize];
        _moveRight(oldData, newData, offset);
        delete[] _data;
        _data = newData;
        _capacity = newSize;
    }

    /**
     * Shifts the data back.
     * @param offset The amount to shift the data right by.
     */
    void _shiftRight(size_t offset)
    {
        // if there's nothing, don't waste time.
        if (_size == 0) return;

        if (offset + _size > _capacity) {
            auto minCapacity = _capacity + offset;
            auto doubleCapacity = nextCapacity();
            auto newSize = minCapacity < doubleCapacity ? doubleCapacity
                                                        : minCapacity;
            _resize(offset, newSize);
        } else {
            _moveRight(_data, _data, offset);
        }
    }

    /**
     * Shifts the data frontward. Elements from [0, offset) are overwritten.
     * @param offset The amount to shift left.
     */
    void _shiftLeft(size_t offset)
    {
        for (size_t i = 0; i < _size; ++i)
        {
            _data[i] = _data[i+offset];
        }
    }

    /**
     * Moves `_size` elements from one array to another.
     * @param from Array to copy from
     * @param to Array to copy to
     * @param offset where `from` goes within `to` (i.e. to[offset] = from[0])
     */
    void _moveRight(T *from, T *to, size_t offset)
    {
        // copy back-to-front to avoid overlap issues
        // special care needed with reverse for loops and unsigned types
        for (size_t remaining = _size; remaining > 0; --remaining) {
            size_t idx = remaining - 1;
            to[idx+offset] = from[idx];
        }
    }

    size_t nextCapacity() const { return _capacity * 2; }

    size_t _capacity = 0;
    T *_data = nullptr;
    size_t _size = 0;
};

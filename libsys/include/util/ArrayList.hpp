//
// Created by Martin Miralles-Cordal on 4/29/2018.
//

#pragma once

#include <util/DynamicArray.hpp>
#include <util/List.hpp>
#include <util/Iterator.hpp>

namespace sys {

/**
 * An ArrayList is an ordered list where the elements are stored in a contiguous
 * array, which is dynamically resized as needed.
 *
 * ArrayLists excel at random access due to the contents being stored in a
 * contiguous block of memory, compared to LinkedLists, which need to traverse
 * node by node to get to any one element. ArrayLists are also faster to iterate
 * though for the same reason. When the needed space is available, insertion and
 * removal of elements at the end of the ArrayList is very fast and requires no
 * further heap allocations.
 *
 * ArrayLists suffer in insertion and removal of elements anywhere far from the
 * end of the list, as the contents need to be shifted to maintain order and
 * contiguousness. Additionally, once reaching the allocated capacity, the
 * memory block needs to be grown if any new elements are to be inserted. This
 * requires allocating a new, larger memory block and copying over the old
 * contents, which is expensive. This is mitigated somewhat by the ArrayList's
 * default behavior of allocating a new block double the size of the current
 * block, allowing many insertions before the next resizing event. This comes at
 * the expense of memory efficiency, as the list takes up more space than
 * strictly necessary. This can be further mitigated if the creator of the list
 * knows beforehand roughly how much space will be needed and can then pre-
 * allocate an appropriate amount of space.
 *
 * @tparam T The type of object the ArrayList holds.
 */
template <typename T>
class ArrayList
{
    class ALIteratorImpl;
  public:
    using value_type = T;
    using iterator = value_type *;
    using const_iterator = value_type const *;

    /** Constructs an ArrayList with the default pre-allocation. */
    ArrayList() : ArrayList(8) {}

    /**
     * Constructs an ArrayList, pre-allocating space for `reserve` number of
     * elements.
     * @param reserve The number of elements to pre-allocate space for.
     */
    ArrayList(size_t reserve) : _data(reserve) {}

    /**
     * Constructs an ArrayList, pre-allocating space for all the provided elements.
     * @param il The elements to pre-allocate space for and insert.
     */
    ArrayList(std::initializer_list<T> il) : _data{il}, _size{il.size()} {}

    /**
     * Reserves enough room to hold the given amount.
     * This function can only grow the ArrayList. If reserve <= capacity, no
     * action is performed.
     * @param reserve The number of elements to make room for.
     */
    void reserve(size_t reserve)
    {
        if (reserve > capacity()) {
            _data.resize(reserve);
        }
    }

    /**
     * Adds an element to the top of the ArrayList.
     * @note This is an O(n) operation! If you're doing this often, consider
     *       the ordering of your data, or use a different List implementation.
     * @param obj The object to add.
     * @return `true` if an object was added. `false` otherwise.
     */
    bool push(const T &obj)
    {
        if constexpr (std::is_copy_constructible<T>::value) {
            shiftOrResize(1);
            _data[0] = obj;
            ++_size;
            return true;
        }

        return false;
    }

    /**
     * Adds an element to the top of the ArrayList.
     * @note This is an O(n) operation! If you're doing this often, consider
     *       the ordering of your data, or use a different List implementation.
     * @param obj The object to add.
     * @return `true` if an object was added. `false` otherwise.
     */
    bool push(T &&obj)
    {
        shiftOrResize(1);
        _data[0] = std::move(obj);
        ++_size;
        return true;
    }

    /**
     * Adds an element to the back of the ArrayList.
     * @param obj The object to add.
     * @return `true` if an object was added. `false` otherwise.
     */
    bool enqueue(const T &obj)
    {
        if constexpr (std::is_copy_constructible<T>::value) {
            if (_size == capacity()) {
                _data.resize();
            }

            _data[_size] = obj;
            ++_size;
            return true;
        }

        return false;
    }

    /**
     * Adds an element to the back of the ArrayList.
     * @param obj The object to add.
     * @return `true` if an object was added. `false` otherwise.
     */
    bool enqueue(T &&obj)
    {
        if (_size == capacity()) {
            _data.resize();
        }

        _data[_size] = std::move(obj);
        ++_size;
        return true;
    }


    /**
     * Removes an element from the top of the ArrayList.
     * @note This is an O(n) operation! If you're doing this often, consider
     *       the ordering of your data, or use a different List implementation.
     * @return The object. If the ArrayList is empty, the return value is undefined.
     */
    value_type pop()
    {
        T ret = std::move(_data[0]);
        _data.shift(1, true);
        --_size;
        return ret;
    }

    /**
     * Removes an element from the back of the ArrayList.
     * @return The object. If the ArrayList is empty, the return value is undefined.
     */
    value_type popBack() { --_size; return std::move(_data[_size]); }

    /**
     * Returns the element at the top of the ArrayList without removing it.
     * @return The object. If the ArrayList is empty, the return value is undefined.
     */
    const value_type &peek() const { return _data[0]; }

    /**
     * Returns the element at the back of the ArrayList without removing it.
     * @return The object. If the ArrayList is empty, the return value is undefined.
     */
    const value_type &peekBack() const { return _data[_size-1]; }

    /**
     * Returns whether or not the ArrayList is empty.
     * @return `true` if empty, `false` otherwise.
     */
    bool isEmpty() const { return _size == 0; }

    /**
     * The number of elements in the ArrayList.
     * @return A size_t equal to the number of elements.
     */
    size_t size() const { return _size; }

    /**
     * Adds the object to the ArrayList, growing its size.
     *
     * @param object The object to add.
     * @return `true` if the ArrayList changed. `false` otherwise.
     */
    bool insert(const T &object) { return enqueue(object); }

    /**
     * Adds the object to the ArrayList at the given index.
     * @note This is an O(n) operation.
     * @param object The object to add.
     * @param idx The index to add it to. Existing elements from idx onward will
     *            be shifted one to the right.
     * @return `true` if the ArrayList changed. `false` otherwise.
     */
    bool insert(const T &object, size_t idx)
    {
        if (idx == _size) {
            return enqueue(object);
        } else if (idx == 0) {
            return push(object);
        }

        // TODO: think about an for hash tables, or if we just allow it
        if (idx > _size) {
            return false;
        }

        // TODO: this is inefficient, as we slide things over twice.
        if (_size == capacity()) {
            _data.resize();
        }

        // move everything at and beyond idx right one.
        for (size_t i = _size; i > idx; --i) {
            _data[i] = _data[i-1];
        }

        _data[idx] = object;
        ++_size;
        return true;
    }

    /**
     * Removes the first instance of the object from the ArrayList.
     * @note This is O(n). If you're doing this often, consider using some other
     *       collection.
     * @param object The object to remove.
     * @return `true` if the ArrayList changed. `false` otherwise.
     */
    bool remove(const T &object)
    {
        size_t removalIdx = SIZE_MAX;
        for (size_t i = 0; i < _size; ++i) {
            if (_data[i] == object) {
                removalIdx = i;
                break;
            }
        }

        return remove(removalIdx);
    }

    /**
     * Removes the object at the given index.
     * @note This is O(n), where n is distance from the end of the ArrayList.
     * @param idx The index to remove from.
     * @return
     */
    bool remove(size_t idx)
    {
        if (idx >= _size) {
            return false;
        }

        --_size;
        // move everything right of idx left one.
        for (size_t i = idx; i < _size; ++i) {
            _data[i] = std::move(_data[i+1]);
        }

        return true;
    }

    /**
     * Returns the element at the given index.  No bounds checking is performed.
     * @param idx The index of the object.
     * @return The object at that index. If the index is out of bounds, the
     *         return value is undefined.
     */
    T const & operator[](size_t idx) const & { return _data[idx]; }

    /**
     * Returns the element at the given index.  No bounds checking is performed.
     * @param idx The index of the object.
     * @return The object at that index. If the index is out of bounds, the
     *         return value is undefined.
     */
    T& operator[](size_t idx) & { return _data[idx]; }

    /**
     * Returns the element at the given index.  No bounds checking is performed.
     * @param idx The index of the object.
     * @return The object at that index. If the index is out of bounds, the
     *         return value is undefined.
     */
    T const && operator[](size_t idx) const && { return _data[idx]; }

    /**
     * Returns the element at the given index.  No bounds checking is performed.
     * @param idx The index of the object.
     * @return The object at that index. If the index is out of bounds, the
     *         return value is undefined.
     */
    T&& operator[](size_t idx) && { return _data[idx]; }

    /** An iterator pointing to the first element in the list. */
    const_iterator begin() const { return _data.begin(); }
    iterator begin() { return _data.begin(); }

    /**
     * An Iterator "past the end" of the list. When encountered, it means there
     * is nothing else to read and iteration should stop.
     *
     * @return An iterator signifying the end of this ArrayList.
     */
    const_iterator end() const { return begin() + _size; }
    iterator end() { return begin() + _size; }

    /**
     * The maximum capacity of the currently allocated block.
     * @return The size of the underlying memory block.
     */
    size_t capacity() const { return _data.capacity(); }

    /**
     * Destroys all elements and resets the reserved block to the default
     * pre-allocation.
     * @return `true`.
     */
    bool clear()
    {
        _data.clear();
        _size = 0;
        return true;
    }

  private:
    static constexpr size_t npos = static_cast<size_t>(-1);
    /**
     * Shifts the data back. Resizes if necessary so no data gets lost.
     * @param offset The amount to shift the data right by.
     */
    void shiftOrResize(size_t offset)
    {
        // if there's nothing, don't waste time.
        if (_size == 0) return;

        if (offset + _size > capacity()) {
            auto minCapacity = capacity() + offset;
            auto doubleCapacity = capacity() * 2;
            auto newSize = minCapacity < doubleCapacity ? doubleCapacity
                                                        : minCapacity;
            _data.resize(newSize, offset);
        } else {
            _data.shift(offset);
        }
    }

    DynamicArray<T> _data;
    size_t _size = 0;
};

static_assert(List<ArrayList<int>>);

} // libsys namespace

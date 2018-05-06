#pragma once

#include <util/DynamicArray.hpp>
#include <util/List.hpp>

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
class ArrayList : public List<T>
{
  public:
    using Iterator = typename Iterable<T>::Iterator;

    /** Constructs an ArrayList with the default pre-allocation. */
    ArrayList() : ArrayList(8) {}

    /**
     * Constructs an ArrayList, pre-allocating space for `reserve` number of
     * elements.
     * @param reserve The number of elements to pre-allocate space for.
     */
    ArrayList(size_t reserve) : _data(reserve) {}

    /**
     * Adds an element to the top of the ArrayList.
     * @note This is an O(n) operation! If you're doing this often, consider
     *       the ordering of your data, or use a different List implementation.
     * @param obj The object to add.
     * @return `true` if an object was added. `false` otherwise.
     */
    bool push(const T &obj) override
    {
        shiftOrResize(1);
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
        if (_size == capacity()) {
            _data.resize();
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
        _data.shift(1, true);
        --_size;
        return ret;
    }

    /**
     * Removes an element from the back of the ArrayList.
     * @return The object. If the ArrayList is empty, the return value is undefined.
     */
    T popBack() override { --_size; return _data[_size]; }

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
    bool isEmpty() const override { return _size == 0; }

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

        // TODO: think about an override for hash tables, or if we just allow it
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
    bool remove(const T &object) override
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
            _data[i] = _data[i+1];
        }

        return true;
    }

    /**
     * Returns the element at the given index.  No bounds checking is performed.
     * @param idx The index of the object.
     * @return The object at that index. If the index is out of bounds, the
     *         return value is undefined.
     */
    T& operator[](size_t idx) const override { return _data[idx]; }

    /** An iterator pointing to the first element in the list. */
    Iterator begin() const override
    {
        auto *llit = new ALIterator{0, this};
        autorelease(llit);
        return Iterable<T>::newIterator(llit);
    }

    /**
     * An Iterator "past the end" of the list. When encountered, it means there
     * is nothing else to read and iteration should stop.
     *
     * @return An iterator signifying the end of this ArrayList.
     */
    Iterator end() const override
    {
        auto *llit = new ALIterator{npos, this};
        autorelease(llit);
        return Iterable<T>::newIterator(llit);
    }

    /**
     * The maximum capacity of the currently allocated block.
     * @return The size of the underlying memory block.
     */
    size_t capacity() const { return _data.capacity(); }

  private:
    static constexpr size_t npos = -1;
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
            _data.resize(offset, newSize);
        } else {
            _data.shift(offset);
        }
    }

    class ALIterator : public Iterable<T>::IteratorImpl
    {
        friend class ArrayList;
        using Iterable<T>::IteratorImpl::publicInstance;
        using Iterable<T>::IteratorImpl::implOf;
      public:
        /**
         * (pre-increment) Advances the iterator one element forward.
         * @return A reference to the next Iterator in the Collection.
         */
        Iterator& operator++() override
        {
            ++_index;
            if (_index == _parent->size()) {
                _index = npos;
            }

            return *publicInstance();
        }

        /**
         * (post-increment) Advances the Iterator one element forward.
         * @return An Iterator equal to this before the increment.
         */
        Iterator operator++(int) override
        {
            ALIterator *it = new ALIterator(*this);
            autorelease(it);
            this->operator++();
            return Iterable<T>::newIterator(it);
        }

        /**
         * Retrieves a reference to the pointed-to element.
         * @return A reference to the element the Iterator is pointing at. If it
         *         isn't pointing to anything (e.g. end of the Collection), the
         *         result is undefined.
         */
        T& operator*() const override { return (*_parent)[_index]; }

        /**
         * Equality operator.
         * @param rhs The iterator to compare to.
         * @return `true` if they are equal, `false` otherwise.
         */
        bool operator==(Iterator const &rhs) const override
        {
            if (classId() != rhs.classId()) {
                return false;
            }

            ALIterator *iterator = reinterpret_cast<ALIterator *>(implOf(rhs));
            if (!iterator) {
                return false;
            }

            if (iterator == this) {
                return true;
            }

            return _index == iterator->_index && _parent == iterator->_parent;
        }

        size_t classId() const override { return 0xA88A1757; }

      private:
        ALIterator(size_t index, ArrayList const *parent)
                : _index(index), _parent(parent)
        {}

        size_t _index;
        ArrayList const *_parent;
    };

    DynamicArray<T> _data;
    size_t _size = 0;
};

#pragma once

#include <util/List.hpp>
#include <cstdio>

namespace _ns_LIBSYS {

/**
 * A LinkedList is an ordered list where each element is contained in a node,
 * and each node has a pointer to the next node which comes after it, forming a
 * chain.
 *
 * LinkedLists are memory efficient in that the size of the list is equal to the
 * LinkedList header and the number of nodes in the list. Array-based stores are
 * costly to grow and shrink, due to their desire to maintain contiguousness,
 * and thus each grow operation grows it by a large amount in anticipation for
 * future insertions. They often don't shrink at all on removal, again to avoid
 * future grow operations.
 *
 * It suffers in random access, as, unlike an array, the elements are not
 * contiguous in memory, and therefore the whole list must be traversed from the
 * beginning in order to find the nth element. It is also, to a much lesser
 * extent, slower to iterate over than an array based store due to the need to
 * dereference and jump all over the heap, whereas arrays are contiguous and may
 * neatly fit in cache lines, etc.
 *
 * @tparam T The type of object the LinkedList holds.
 */
template <typename T> class LinkedList
{
    class LLIteratorImpl;
  public:
    using ValueType = T;
    using iterator = Iterator<LLIteratorImpl>;
    using const_iterator = ConstIterator<LLIteratorImpl>;

    /**
     * Adds an element to the front of the List.
     * @param obj The object to add.
     * @return `true` if an object was added. `false` otherwise.
     */
    bool push(const T &obj)
    {
        if constexpr (std::is_copy_constructible<T>::value) {
            if (!_first) {
                _first = make_arc<Node>(obj);
                _last = _first;
            } else {
                _first = _first->insertBefore(obj);
            }

            ++_size;

            return true;
        } else {
            return false;
        }
    }

    /**
     * Adds an element to the front of the List.
     * @param obj The object to add.
     * @return `true` if an object was added. `false` otherwise.
     */
    bool push(T &&obj)
    {
        if (!_first) {
            _first = make_arc<Node>(std::move(obj));
            _last = _first;
        } else {
            _first = _first->insertBefore(std::move(obj));
        }

        ++_size;

        return true;
    }

    bool insert(T const &obj) { return push(obj); }
    bool insert(T &&obj) { return push(std::move(obj)); }

    /**
     * Removes an element from the front of the List.
     * @return The object. If the List is empty, the return value is undefined.
     */
    T pop()
    {
        auto oldFirst = _first;

        if (_first == _last) {
            _first = nullptr;
            _last = nullptr;
        } else {
            _first = _first->remove()->next;
        }

        if (_size) {
            --_size;
        }

        return std::move(oldFirst->value);
    }

    /**
     * Returns the element at the front of the List without removing it.
     * @return The object. If the List is empty, the return value is undefined.
     */
    const T &peek() const { return _first->value; }

    /**
     * Adds an element to the back of the List.
     * @param obj The object to add.
     * @return `true` if an object was added. `false` otherwise.
     */
    bool enqueue(const T &obj)
    {
        if constexpr (std::is_copy_constructible<T>::value) {
            if (!_last) {
                return push(obj); // list is empty
            }

            _last = _last->insertAfter(obj);
            ++_size;
            return true;
        } else {
            return false;
        }
    }

    bool enqueue(T &&obj)
    {
        if (!_last) {
            return push(std::move(obj)); // list is empty
        }

        _last = _last->insertAfter(std::move(obj));
        ++_size;
        return true;
    }

    /**
     * Removes an element from the back of the List.
     * @return The object. If the List is empty, the return value is undefined.
     */
    T popBack()
    {
        auto oldLast = _last;

        if (_first == _last) {
            _first = nullptr;
            _last = nullptr;
        } else {
            _last = _last->remove()->prev;
        }

        if (_size) {
            --_size;
        }

        return std::move(oldLast->value);
    }

    /**
     * Returns the element at the back of the List without removing it.
     * @return The object. If the List is empty, the return value is undefined.
     */
    const T &peekBack() const { return _last->value; }

    /**
     * Returns whether or not the List is empty.
     * @return `true` if empty, `false` otherwise.
     */
    bool isEmpty() const { return _first == nullptr; }

    /**
     * The number of elements in the List.
     * @return A size_t equal to the number of elements.
     */
    size_t size() const { return _size; }

    /**
     * Removes the first element equal to `object`.
     * @param object The object to remove.
     * @return `true` if something was removed, false otherwise.
     */
    bool remove(const T &object)
    {
        if (!_first) {
            return false; // no objects
        }

        auto it = _first;
        while (it != nullptr) {
            if (it->value == object) {
                auto target = it->remove();
                if (target == _first) _first = target->next;
                if (target == _last) _last = target->prev;
                --_size;
                return true;
            }

            it = it->next;
        }

        return false;
    }

    /**
     * Removes all elements from the LinkedList.
     * @return `true`.
     */
    bool clear()
    {
        _first = nullptr;
        _last = nullptr;
        _size = 0;
        return true;
    }

    /**
     * Returns the element at the given index. No bounds checking is performed.
     *
     * @note This is a linked list, so this operation is O(n)!
     * @param idx The index of the object.
     * @return The object at that index. If the index is out of bounds, the
     *         return value is undefined.
     */
    T& operator[](size_t idx) const
    {
        Node *it = _first.get();
        for (; idx > 0; --idx) {
            it = it->next.get();
        }

        return it->value;
    }

    /**
     * Checks if two LinkedList objects are equal.
     * @param rhs The LinkedList to compare to.
     * @return `true` if equal, `false` otherwise.
     */
    bool operator==(const LinkedList &rhs) const
    {
        return _first == rhs._first && _last == rhs._last && _size == rhs._size;
    }

    /**
     * Checks if two LinkedList objects are not equal.
     * @param rhs The LinkedList to compare to.
     * @return `true` if not equal, `false` otherwise.
     */
    bool operator!=(const LinkedList &rhs) const { return !operator==(rhs); }

    /** An iterator pointing to the first element in the list. */
    /** @{ */
    const_iterator begin() const { return const_iterator{LLIteratorImpl{_first.get(), this}}; }
    iterator begin() { return iterator{LLIteratorImpl{_first.get(), this}}; }
    /** @} */

    /**
     * An Iterator "past the end" of the list. When encountered, it means there
     * is nothing else to read and iteration should stop.
     *
     * @return An iterator signifying the end of this LinkedList.
     */
     /** @{ */
    const_iterator end() const { return const_iterator{LLIteratorImpl{nullptr, this}}; }
    iterator end() { return iterator{LLIteratorImpl{nullptr, this}}; }
    /** @} */

  private:
    struct Node
    {
        Node(const T &v) : value(v) {}
        Node(T &&v) : value(std::move(v)) {}

        /**
         * Inserts an object into the List positioned before this node.
         * @param obj The object to add.
         * @return The newly created node.
         */
        ArcPtr<Node> insertBefore(const T &obj)
        {
            auto n = make_arc<Node>(obj);
            n->next = this;
            n->prev = prev;
            if (prev) prev->next = n;
            prev = n.get();
            return n;
        }

        /**
         * Inserts an object into the List positioned before this node.
         * @param obj The object to add.
         * @return The newly created node.
         */
        ArcPtr<Node> insertBefore(T &&obj)
        {
            auto n = make_arc<Node>(std::move(obj));
            n->next = this;
            n->prev = prev;
            if (prev) prev->next = n;
            prev = n.get();
            return n;
        }


        /**
         * Inserts an object into the List positioned after this node.
         * @param obj The object to add.
         * @return The newly created node.
         */
        ArcPtr<Node> insertAfter(const T &obj)
        {
            auto n = make_arc<Node>(obj);
            n->prev = this;
            n->next = next;
            if (next) next->prev = n.get();
            next = n;
            return n;
        }

        /**
         * Inserts an object into the List positioned after this node.
         * @param obj The object to add.
         * @return The newly created node.
         */
        ArcPtr<Node> insertAfter(T &&obj)
        {
            auto n = make_arc<Node>(std::move(obj));
            n->prev = this;
            n->next = next;
            if (next) next->prev = n.get();
            next = n;
            return n;
        }

        /**
         * Removes itself from the list. :(
         * @return ArcPtr to itself.
         */
        ArcPtr<Node> remove()
        {
            // store this in a ArcPtr in case our neighbors are our sole owners
            ArcPtr<Node> self{this};
            if (prev) prev->next = next;
            if (next) next->prev = prev;
            return self;
        }

        T value;
        Node *prev = nullptr;
        ArcPtr<Node> next = nullptr;
    };

    class LLIteratorImpl
    {
      public:
        using ValueType = LinkedList::ValueType;

        void increment() { _obj = _obj->next.get(); }
        void decrement() { _obj = _obj->prev; }
        T& get_value() const { return _obj->value; }
        T* get_ptr() const { return &_obj->value; }
        bool operator==(LLIteratorImpl const &rhs) const { return _obj == rhs._obj && _parent == rhs._parent; }
        constexpr bool operator!=(LLIteratorImpl const &rhs) const { return !(rhs == *this); }

      private:
        LLIteratorImpl(Node *node, LinkedList const *parent) : _obj(node), _parent(parent) {}

        Node *_obj;
        LinkedList const *_parent;
    };

    ArcPtr<Node> _first = nullptr;
    ArcPtr<Node> _last = nullptr;
    size_t _size = 0;
};


namespace concept_check {
using namespace concepts;
template <typename T> requires List<LinkedList<T>> using LinkedListCheck = LinkedList<T>;
static_assert(std::default_constructible<LinkedListCheck<int>>, "LinkedList does not meet the requirements of List.");
}

} // libsys namespace

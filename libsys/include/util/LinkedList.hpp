#pragma once

#include <util/List.hpp>

template <typename T> class LinkedList : public virtual List<T>
{
  public:
    LinkedList() {}

    /**
     * Adds an element to the front of the List.
     * @param obj The object to add.
     * @return `true` if an object was added. `false` otherwise.
     */
    bool push(const T &obj) override
    {
        if (!_first) {
            _first = ArcPtr<Node>::make(obj);
            _last = _first;
        } else {
            auto oldFirst = _first;
            _first = ArcPtr<Node>::make(obj);
            _first->next = oldFirst;
        }

        ++_size;

        return true;
    }

    /**
     * Removes an element from the front of the List.
     * @return The object. If the List is empty, the return value is undefined.
     */
    T pop() override
    {
        auto oldFirst = _first;

        if (_first == _last) {
            _first = nullptr;
            _last = nullptr;
        } else {
            _first = _first->next;
        }

        if (_size) {
            --_size;
        }

        return oldFirst->value;
    }

    /**
     * Returns the element at the front of the List without removing it.
     * @return The object. If the List is empty, the return value is undefined.
     */
    T peek() const override
    {
        return _first->value;
    }

    /**
     * Adds an element to the back of the List.
     * @param obj The object to add.
     * @return `true` if an object was added. `false` otherwise.
     */
    bool enqueue(const T &obj) override
    {
        if (!_last) {
            return push(obj); // list is empty
        }

        _last->next = ArcPtr<Node>::make(obj);
        _last = _last->next;
        ++_size;
        return true;
    }

    /**
     * Returns whether or not the List is empty.
     * @return `true` if empty, `false` otherwise.
     */
    bool isEmpty() const { return _first == nullptr; }

    /**
     * The number of elements in the List.
     * @return A size_t equal to the number of elements.
     */
    virtual size_t size() const
    {
        return _size;
    }

    /**
     * Removes the first element equal to `object`.
     * @param object The object to remove.
     * @return `true` if something was removed, false otherwise.
     */
    bool remove(const T &object) override
    {
        if (!_first) {
            return false; // no objects
        }

        auto it = _first;
        if (_first->value == object) {
            _first = _first->next;
            --_size;
            return true;
        }

        while (it->next != nullptr) {
            if (it->next->value == object) {
                auto next = it->next;
                it->next = next->next;
                --_size;
                return true;
            }
            it = it->next;
        }

        return false;
    }

    /**
     * Returns the element at the given index. No bounds checking is performed.
     *
     * @note This is a linked list, so this operation is O(n)!
     * @param idx The index of the object.
     * @return The object at that index. If the index is out of bounds, the
     *         return value is undefined.
     */
    T& operator[](size_t idx) const override
    {
        Node *it = _first.get();
        for (; idx > 0; --idx) {
            it = it->next.get();
        }

        return it->value;
    }

  private:
    struct Node : public Object {
        Node(const T &v) : value(v) {}
        ~Node() { puts("Node destroyed!"); }
        T value;
        ArcPtr<Node> next = nullptr;
    };

    ArcPtr<Node> _first = nullptr;
    ArcPtr<Node> _last = nullptr;
    size_t _size = 0;
};
#pragma once

#include <util/Queue.hpp>

#include <stdint.h>
#include <utility>

/**
 * A RingBuffer is a Queue of fixed size. If an object is enqueued while the
 * queue is full, the frontmost (oldest) object is erased to make room.
 * @tparam T The type of object to store. Since the space for the whole buffer
 *           is reserved up front, T must be default-constructible.
 */
template <typename T>
class RingBuffer : public virtual Queue<T>
{
public:
    explicit RingBuffer(size_t capacity)
            : _capacity(capacity), _front(0), _back(0), _size(0)
            , _buffer(new T[capacity])
    {}

    /**
     * Adds an element to the back of the queue. If the buffer is full, the
     * frontmost element is removed in order to make space. (The newly enqueued
     * item still goes in the back.)
     * @param item The item to enqueue.
     * @return `true` if the operation succeeded, `false` otherwise.
     */
    bool enqueue(const T &item) override
    {
        _buffer[_back] = item;
        _back = _next(_back);
        if(_front == _back) {
            _front = _next(_front);
        } else {
            ++_size;
        }

        return true;
    }

    bool enqueue(T &&item) override
    {
        _buffer[_back] = std::move(item);
        _back = _next(_back);
        if(_front == _back) {
            _front = _next(_front);
        } else {
            ++_size;
        }

        return true;
    }

    /**
     * Removes the frontmost element from the queue and returns it.
     * @return The frontmost element. If the queue is empty, the return value is
     *         undefined.
     */
    T pop() override
    {
        if(isEmpty()) {
            return T{};
        } else {
            int retIndex = _front;
            _front = _next(_front);
            --_size;
            return _buffer[retIndex];
        }
    }

    /**
     * Returns the frontmost element in the queue.
     * @return The frontmost element. If the queue is empty, the return value is
     *         undefined.
     */
    const T &peek() const override { return isEmpty() ? *(T*)nullptr : _buffer[_front]; }

    /**
     * Returns whether or not the collection is empty.
     * @return `true` if empty, `false` otherwise.
     */
    bool isEmpty() const override { return size() == 0; }

    /**
     * Returns the number of elements in the buffer.
     * @return The number of elements in the buffer.
     */
    size_t size() const override { return _size; }

    /**
     * The maximum capacity of the buffer.
     * @return The maximum capacity of the buffer.
     */
    size_t capacity() const { return _capacity; }

    /** Empties the buffer. */
    bool clear() override
    {
        _front = _back;
        return true;
    }

private:
    size_t _next(size_t current) { return (current + 1) % _capacity; }
    size_t _capacity;
    uint32_t _front;
    uint32_t _back;
    size_t _size;
    T *_buffer;
};

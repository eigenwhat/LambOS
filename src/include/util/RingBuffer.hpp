#pragma once
#include <util/Queue.hpp>

template <typename T>
class RingBuffer : public virtual Queue<T>
{
public:
    explicit RingBuffer(size_t capacity) : _capacity(capacity), _front(0), _back(0), _size(0) { _buffer = new T[capacity]; }
    bool enqueue(T item) {
        _buffer[_back] = item;
        _back = _next(_back);
        if(_front == _back) {
            _front = _next(_front);
        } else {
            ++_size;
        }

        return true;
    }
    T dequeue() {
        if(isEmpty()) {
            return 0;
        } else {
            int retIndex = _front;
            _front = _next(_front);
            --_size;
            return _buffer[retIndex];
        }
    }
    T peek() { 
        if(isEmpty()) {
            return 0;
        } else {
            return _buffer[_front];
        }
    }

    bool isEmpty() {
        return size() == 0;
    }

    size_t size() { return _size; }

    void clear() {
        _front = _back;
    }

    bool remove(T) {
        return false; // can't delete from buffer!
    }

private:
    size_t _next(size_t current) { return (current + 1) % _capacity; }
    size_t _capacity;
    uint32_t _front;
    uint32_t _back;
    size_t _size;
    T *_buffer;
};

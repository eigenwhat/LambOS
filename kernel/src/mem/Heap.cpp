//
// Created by Martin Miralles-Cordal on 5/19/2018.
//

#include <mem/Heap.hpp>

std::uintptr_t Heap::grow(std::uintptr_t size) noexcept
{
    auto oldEnd = _end;
    _end += size;
    return oldEnd;
}

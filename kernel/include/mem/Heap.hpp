//
// Created by Martin Miralles-Cordal on 5/19/2018.
//

#pragma once

#include <cstddef>
#include <cstdint>

/** A process's free store (heap memory). */
class Heap
{
  public:
    constexpr explicit Heap(std::uintptr_t begin) noexcept : _begin(begin), _end(begin) {}

    /**
     * Grows the heap by the given amount.
     * This provides the functional equivalent of `sbrk()`.
     * @param size The amount to grow the heap by.
     * @return The previous end of the heap, or UINTPTR_MAX (-1) on error.
     */
    std::uintptr_t grow(std::uintptr_t size) noexcept;

    /** The begin address of the heap. */
    [[nodiscard]] constexpr std::uintptr_t begin() const noexcept { return _begin; }

    /** The end address of the heap. */
    [[nodiscard]] constexpr std::uintptr_t end() const noexcept { return _end; }

    /** The size of the heap, in bytes. */
    [[nodiscard]] constexpr std::ptrdiff_t size() const noexcept { return static_cast<std::ptrdiff_t>(_end - _begin); }

  private:
    std::uintptr_t _begin = UINTPTR_MAX; ///< Beginning of the heap.
    std::uintptr_t _end = 0;             ///< End of the heap. (unaligned)
    std::uintptr_t _alignedEnd = 0;      ///< _end rounded up to nearest page boundary.
    std::uintptr_t _limit = UINTPTR_MAX; ///< Max value _alignedEnd allowed to reach.
};

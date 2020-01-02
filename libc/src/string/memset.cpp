//
// Created by Martin Miralles-Cordal on 01/01/2020.
//

#include <stddef.h>
#include <stdint.h>

#include <decl.h>

namespace {

using byte = unsigned char;

template <size_t Dividend> constexpr inline size_t shift_div();

template <> constexpr inline size_t shift_div<1>() { return 0; }
template <> constexpr inline size_t shift_div<2>() { return 1; }
template <> constexpr inline size_t shift_div<4>() { return 2; }
template <> constexpr inline size_t shift_div<8>() { return 3; }

template <typename Word> constexpr inline size_t word_align_mask = sizeof(Word) - 1;

template <typename Word, typename T>
constexpr inline bool is_word_aligned(T *p) { return 0 == (reinterpret_cast<uintptr_t>(p) & word_align_mask<Word>); }


constexpr inline byte *memset_b_affix(byte *ptr, byte value, size_t num)
{
    while(num--) { *ptr++ = value; }
    return ptr;
}

template <typename Word>
void *memset_w(void * const ptr, int value, size_t const num)
{
    byte const bvalue = static_cast<byte>(static_cast<unsigned int>(value) & 0xffu);
    size_t const head_sz = reinterpret_cast<uintptr_t>(ptr) & 0b11u;
    size_t const body_sz = (num - head_sz);
    size_t const tail_sz = body_sz & word_align_mask<Word>;

    // first, do any unaligned bytes at the front
    auto word_ptr = reinterpret_cast<Word*>(memset_b_affix(static_cast<byte *>(ptr), bvalue, head_sz));

    // copy first byte over the rest of the word's bytes
    Word const wvalue = [value] {
        auto v = static_cast<Word>(value);
        if constexpr (sizeof(Word) >= 2) { v |= v << 8; }
        if constexpr (sizeof(Word) >= 4) { v |= v << 16; }
        if constexpr (sizeof(Word) >= 8) { v |= v << 32; }
        return v;
    }();

    // now, do the main (word-aligned) body
    size_t words_num = body_sz >> shift_div<sizeof(Word)>();
    while (words_num--) { *word_ptr++ = wvalue; }

    // finally, do the remaining unaligned bytes at the end
    memset_b_affix(reinterpret_cast<byte *>(word_ptr), bvalue, tail_sz);

    return ptr;
}

} // namespace

__BEGIN_DECLS

//void *memset(void *ptr, int value, size_t num)
//{
//    uint8_t *dst = (uint8_t *)ptr;
//    for(; num--; *dst++ = (uint8_t)value);
//
//    return ptr;
//}

void *memset(void *ptr, int value, size_t num) { return memset_w<int>(ptr, value, num); }

__END_DECLS

//
// Created by Martin Miralles-Cordal on 01/01/2020.
//

#pragma once

#include <mem/Units.hpp>

namespace X86 {

using namespace sys::mem_unit_literals;
constexpr std::size_t const kPageFrameSize = 4_KiB;
constexpr std::size_t const kPageTableSize = 4_KiB;
constexpr std::size_t const kPageDirectorySize = kPageTableSize * 1024;

}


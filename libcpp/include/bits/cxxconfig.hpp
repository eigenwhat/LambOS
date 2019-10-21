//
// Created by Martin Miralles-Cordal on 10/20/19.
//

#pragma once

#if __cpp_noexcept_function_type
#define NOEXCEPT_TPARAM , bool Nothrow
#define NOEXCEPT_QUALIFIER noexcept (Nothrow)
#else
#define NOEXCEPT_TPARAM
#define NOEXCEPT_QUALIFIER
#endif
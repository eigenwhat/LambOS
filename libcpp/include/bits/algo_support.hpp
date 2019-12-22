//
// Created by Martin Miralles-Cordal on 12/22/2019.
//

#pragma once

#define __DEF_NIEBLOID(type, name)      \
    inline namespace function_objects { \
    inline constexpr type name{};       \
    }

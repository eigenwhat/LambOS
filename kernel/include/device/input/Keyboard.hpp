//
// Created by Martin Miralles-Cordal on 9/16/2013.
//

#pragma once

#include <Object.hpp>
#include <sys/keys.h>

class Keyboard : public Object
{
public:
    virtual KeyEvent read() = 0;
    virtual bool keyIsPressed(KeyCode key) const = 0;
};

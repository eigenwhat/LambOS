#pragma once
#include <Object.hpp>
#include <sys/keys.h>

class Keyboard : public Object
{
public:
    virtual KeyEvent read() = 0;
};

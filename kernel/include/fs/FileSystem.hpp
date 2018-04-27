#pragma once

#include <device/storage/AtaDevice.hpp>

class FileSystem
{
  public:
    char const *name() { return _name; }

    virtual bool hasFileSystem(AtaDevice &device) = 0;

  protected:
    FileSystem(char const *name) : _name(name) {}

  private:
    char const *_name = nullptr;
};
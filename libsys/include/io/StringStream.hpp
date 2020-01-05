//
// Created by Martin Miralles-Cordal on 01/05/2020.
//

#pragma once

#include <io/OutputStream.hpp>
#include <util/String.hpp>

namespace sys {

class StringStream final : public OutputStream
{
  public:
    StringStream() = default;
    StringStream(String &&str) : _string{std::move(str)} {}
    StringStream(char const *str) : _string{str} {}

    void write(std::byte byte) override { _string.append(static_cast<char>(byte)); }

    String string() const { return _string; }

  private:
    String _string;
};

} // libsys namespace

//
// Created by Martin Miralles-Cordal on 5/6/2018.
//

#include <util/StringTokenizer.hpp>

namespace sys {

StringTokenizer::StringTokenizer(String const &str, String const &delimiters)
        : _string(str), _delimiters(delimiters)
{}

bool StringTokenizer::hasNextToken() const
{
    return tokenStart(_currentIndex) != _string.size();
}

String StringTokenizer::nextToken()
{
    _currentIndex = tokenStart(_currentIndex);
    if (_currentIndex == _string.size()) {
        return String::emptyString();
    }

    auto start = _currentIndex;
    auto end = tokenEnd(start) + 1;
    _currentIndex = end;
    return _string.substring(start, end - start);
}

void StringTokenizer::reset(String const &str)
{
    _string = str;
    _currentIndex = 0;
}

size_t StringTokenizer::tokenStart(size_t startIndex) const
{
    for (size_t i = startIndex; i < _string.size(); ++i) {
        bool isDelim = false;
        for (size_t d = 0; d < _delimiters.size(); ++d) {
            if (_string[i] == _delimiters[d]) {
                isDelim = true;
                break;
            }
        }

        if (!isDelim) {
            return i;
        }
    }

    return _string.size();
}


size_t StringTokenizer::tokenEnd(size_t startIndex) const
{
    for (size_t i = startIndex; i < _string.size(); ++i) {
        for (size_t d = 0; d < _delimiters.size(); ++d) {
            if (_string[i] == _delimiters[d]) {
                return i-1;
            }
        }
    }

    return _string.size() - 1;
}

} // libsys namespace

#include <fs/Path.hpp>
#include <util/StaticList.hpp>
#include <util/StringTokenizer.hpp>

namespace {

static constexpr char const kDelims[] = "/\\";

bool isAbsolutePath(char const *str)
{
    for (auto c : kDelims) {
        if (str[0] == c) return true;
    }

    return false;
}

}

Path::Path(char const *str)
{
    String s {str};
    if (!s.size()) {
        return;
    }

    _isAbsolute = isAbsolutePath(str);

    StringTokenizer tokenizer {s, kDelims};
    while (tokenizer.hasNextToken()) {
        _pathComponents.enqueue(tokenizer.nextToken());
    }
}

String Path::pathString() const
{
    String result;

    if (_isAbsolute) {
        result.append('/');
    }

    for (const String &c : _pathComponents) {
        result.append(c);
        if (c != _pathComponents.peekBack()) {
            result.append('/');
        }
    }

    return result;
}

String const &Path::lastPathComponent() const
{
    if (_pathComponents.isEmpty()) {
        return String::emptyString();
    }

    return _pathComponents.peekBack();
}

String Path::extension() const
{
    if (_isDirectory) { return String::emptyString(); }

    auto &s = lastPathComponent();
    if (!s.size()) { return String::emptyString(); }

    StringTokenizer tokenizer {s, "."};
    String token;
    while (tokenizer.hasNextToken()) {
        token = tokenizer.nextToken();
    }

    return token;
}

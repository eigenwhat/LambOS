#pragma once

#include <util/String.hpp>
#include <Object.hpp>

class StringTokenizer : public Object
{
  public:
    /**
     * Creates an object which encapsulates the tokenization of a String.
     * @param str The String to tokenize.
     * @param delimiters The delimiters to tokenize on. Defaults to a space.
     */
    StringTokenizer(String const &str, String const &delimiters = " ");

    /**
     * Returns whether or not there are more tokens.
     * @return `true` if there are more tokens, `false` otherwise.
     */
    bool hasNextToken() const;

    /**
     * Returns the next token, or an empty string if there are no more tokens.
     * @return A String equal to the next token.
     */
    String nextToken();

    /**
     * Resets the StringTokenizer with a new String.
     * @param str The String to reset the StringTokenizer with.
     */
    void reset(String const &str);

  private:
    /**
     * First half of strtok operation. Returns the index of the first character
     * that *ISN'T* one of the delimiters.
     * @param startIndex Index to start looking at.
     * @return The start index for the next token, or `_string.size()`.
     */
    size_t tokenStart(size_t startIndex) const;

    /**
     * Second half of strtok operation. Returns the index just before the first
     * character that matches one of the delimiters or the null terminator,
     * whichever is encountered first.
     * @param startIndex The index to start looking. Should be the output of
     * tokenStart().
     * @return The index to the last character in the token.
     */
    size_t tokenEnd(size_t startIndex) const;

    String _string;
    String _delimiters;
    size_t _currentIndex = 0;
};

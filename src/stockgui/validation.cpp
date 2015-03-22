/**
 * @file
 * This file contains user input validation routines. 
 *
 * @note It is in a separate compilation unit, as accessing the regex functions
 * adds considerably to the compile time.
 */

/*
The MIT License (MIT)

Copyright (c) 2015 David Bradshaw

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <string>
#include <regex>

#include "validation.h"

using std::string;
using std::regex;

/**
 * Validates that the given string can be converted into a double precision
 * floating point number by the runtime library. 
 *
 * @param input The string to validate
 * @return true if validation succeeded, false otherwise
 */
bool validate_double(const string& input)
{
    regex ve("^-?[0-9]+\\.?[0-9]+$");
    return std::regex_match(input,ve);
}

/**
 * Validates that the given string can be converted into a double precision
 * floating point number by the runtime library, and that the resulting value is
 * greater than zero.  
 *
 * @param input The string to validate
 * @return true if validation succeeded, false otherwise
 */
bool validate_positive_double(const string& input)
{
    regex ve("^[0-9]*\\.?[0-9]+$");
    return std::regex_match(input,ve);
}

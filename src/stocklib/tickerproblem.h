/**
 * @file
 * Public header for the tickerproblem class. This class extends urlproblem to
 * fetch stock price information from the Yahoo! API.
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

#ifndef TICKERPROBLEM_H
#define TICKERPROBLEM_H

#include <string>
#include <functional>

#include <stocklib/stock-task-modes.h>
#include "urltask.h"

class tickerproblem : public urlproblem
{
public:
    tickerproblem( const std::string&, sl_test_behavior_t);

protected:

    virtual std::string decode_response(const std::string&);
    virtual std::string preprocess_url(const std::string&);
    virtual void fetch(buffer&, const std::string&);
private:

    const sl_test_behavior_t _behavior;
    static const std::string _url_template;
    static const std::string _notfound_response;
    static const std::string _fake_response;
    const std::string _ticker;
};

#endif

/**
 * @file
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

#ifndef STOCK_FETCHER_H
#define STOCK_FETCHER_H

/**
 * Fetches a stock price from Yahoo! Finance.
 * This thread class fetches the current bid, ask prices of a stock
 */
class stock_fetcher : public worker_base
{
public:
    virtual void do_work(int);
    virtual void set_result_callback( std::function<void(std::string)> f );
protected:
    static size_t rx_data(void *rx_buffer, size_t size, size_t nmemb, void *local_buffer);
    void deliver_result(const std::string& s); 

    std::function<void(std::string)> result_callback;
    bool hasResultCallback=false;
};

#endif

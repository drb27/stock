/**
 * @file 
 *
 * Very simply command line program that retrieves the latest price of
 * the given ticker symbol. 
 *
 * Example usage:
 * @code
 * me@mymachine ~/ $ stock AAPL
 * @endcode
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

#include <config.h>
#include <iostream>
#include <string>

#include <curl/curl.h>
#include "stocklib.h"

using std::cout;
using std::endl;
using std::string;

string g_ticker="AAPL";

int main( int argc, char* argv[] )
{
    /* Extract symbol from command line, if supplied */
    if (argc > 1 )
	g_ticker = argv[1];

    /* Initialise the CURL library */
    curl_global_init(CURL_GLOBAL_ALL);

    /* Initialise the stocklib library */
    stocklib_init();

    /* Fetch a result */
    char buffer[SL_MAX_BUFFER];
    
    if (SL_OK != stocklib_fetch_synch(g_ticker.c_str(),buffer) )
    {
	cout << "An error occurred." << endl;
	return 1;
    }

    /* Output the result */
    cout << buffer << endl;

    /* Done! */
    return 0;
}

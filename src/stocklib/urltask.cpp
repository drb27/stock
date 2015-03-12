/**
 * @file
 * The implementation of the urltask class
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

#include <regex>
#include <functional>
#include <map>
#include <curl/curl.h>
#include "buffer.h"
#include "sweepup.h"
#include "deathrattle.h"

#include "urltask.h"

using std::string;
using std::regex;
using std::function;
using std::map;

/**
 * @class urlproblem
 * Implementation of a contained problem which fetches a URL from a remote
 * server.  
 *
 * The class is designed to be used with the task class template, which can
 * execute the request either synchronously or asynchronously. 
 * 
 * The class also has provision for modifying the URL before execution, and
 * decoding the response. See preprocess_url() and decode_response()
 * accordingly.
 */

/**
 * Regular constructor. Pass in a URL. Note that the URL will be processed via a
 * call to preprocess_url() immediately prior to the execution of the
 * request. In this class, the URL is not modified. However, in derived classes,
 * preprocess_url() may alter the URL. 
 *
 * @param url The url to use for the request (may be modified first by
 * preprocess_url() )
 */
urlproblem::urlproblem(const std::string& url) :
    contained_problem<string,map<string,string>>(url)
{
}

/**
 * Performs the work of translating the URL, then fetching and decoding the
 * response. 
 *
 * @param url The url which was originally passed to the constructor. 
 * @return The decoded response from the server. 
 */
map<string,string> urlproblem::do_work(string url)
{
    /* Allocate a buffer to receive the response */
    buffer rxbuffer(1024);

    /* Preprocess the URL */
    string processedUrl = preprocess_url(url);

    /* Execute the request */
    fetch(rxbuffer,processedUrl);

    /* Decode and return the response */
    return decode_response(rxbuffer.contents());
}

void urlproblem::fetch(buffer& b, const std::string& url)
{
    perform_query(b,url);
}

string urlproblem::preprocess_url(const string& url)
{
    return url;
}

map<string,string> urlproblem::decode_response(const string& response)
{
    map<string,string> m;
    m["response"] = response;
    return m;
}

void urlproblem::perform_query(buffer& b, const string& url)
{

    /* Get a handle to the easy curl interface, and load the URL */
    auto handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());

    /* Set up the callback and write buffer */
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &rx_data);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &b);
	
    /* Fetch the data */
    curl_easy_perform(handle);
	
    /* Clean-up */
    curl_easy_cleanup(handle);
	
    /* Zero-terminate the data */
    static const char terminate = '\0';
    b.append(&terminate,1);

}

size_t urlproblem::rx_data(void *rx_buffer, 
			size_t size, size_t nmemb, void *local_buffer)
{
    buffer* pBuffer = reinterpret_cast<buffer*>(local_buffer);
    pBuffer->append(rx_buffer,nmemb);
    return size;
}

/**
 * @class urltask
 * A task-derived class that can fetch data from a remote URL, either
 * synchronously or asycnhronously. 
 *
 * The class has some provision for constructing a URL from parameters before
 * doing the fetch, and decoding the returned information. Users of the class
 * (i.e. authors of child classes) need to provide the algorithms for these
 * operations, otherwise everything else is automated. 
 * 
 */

/**
 * Default constructor.
 *
 * @param url A fully formed URL
 */
urltask::urltask(const string& url) : task<string,map<string,string>>( new urlproblem(url) )
{
}

/**
 * Alternative constructor. Allows the use of subclasses of urlproblem.
 *
 * @param pUrlProblem a pointer to a fully constructed urlproblem object
 */
urltask::urltask(urlproblem* pUrlProblem) : task<string,map<string,string>>( pUrlProblem )
{
}

/**
 * Registers a functor to be called when the URL query completes. 
 *
 * @param c The functor to call
 * @param data Application-defined pointer to relevant data
 */
void urltask::set_completion_callback( callback* c, void* data )
{
    auto lock = state.obtain_lock();

    _callback_fn = function<callback>(c);
    _callback_data = data;
    
    state.set_entry_function( TaskState::Finished,
			      [this]() { this->notify_callback(); } );

    if (state.get_state()==urltask::TaskState::Finished)
	notify_callback();
}

void urltask::notify_callback()
{
    _callback_fn(this,_callback_data);
}


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

#include <regex>
#include <functional>
#include <curl/curl.h>
#include <jansson.h>
#include "buffer.h"
#include "sweepup.h"
#include "deathrattle.h"

#include "stock-task.h"

using std::string;
using std::regex;
using std::function;

namespace
{
    const std::string g_ticker="AAPL";

    const std::string cURL = "https://query.yahooapis.com/v1/public/yql?q=select%20Name,LastTradePriceOnly%20from%20yahoo.finance.quotes%20where%20symbol%20%3D%22{STOCK}%22&format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys&callback=";

    const std::string notfound_response = 
	"{\"query\":{\"count\":0,\"created\":\"2015-03-06T11:53:00Z\", \
        \"lang\":\"en-US\",\"results\":null}}";

    size_t rx_data(void *rx_buffer, size_t size, size_t nmemb, void *local_buffer)
    {
	buffer* pBuffer = reinterpret_cast<buffer*>(local_buffer);
	pBuffer->append(rx_buffer,nmemb);
	return size;
    }

    void perform_query(const std::string& url, buffer& b)
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

    std::string parse_response(const buffer& b)
    {
	/* Ensures all memory is freed correctly, even if an exception is thrown */    
	sweepup<json_t*> trash( [](json_t* obj) { json_decref(obj); }  );

	/* Decode the data */
	json_error_t error;
	json_t* root = json_loads(b.contents(), 0, &error );

	std::string output("");

	if (root)
	{
	    /* json_loads() returns a new reference, and so must be freed later */
	    trash.add(root);

	    /* Note that json_object_get() returns a borrowed ref, no need to free */
	    auto query = json_object_get(root,"query");

	    if (query && json_is_object(query) )
	    {
		auto results = json_object_get(query,"results");

		if (results && json_is_object(results) )
		{
		    auto quote = json_object_get(results,"quote");

		    if (quote && json_is_object(quote) )
		    {
			auto bid = json_object_get(quote,"LastTradePriceOnly");
			
			if ( bid && json_is_string(bid) )
			{
			    auto str_bid = json_string_value(bid);
	    
			    output = str_bid;
			    return output;
			}
		    }
		}
	    }
	}

	throw std::logic_error("No stock result found in response");
    }

    std::string fetch(const string ticker)
    {

	/* Format the URL */
	regex stock_exp("\\{STOCK\\}"); 
	auto formattedUrl = std::regex_replace(cURL,stock_exp,ticker, 
					       std::regex_constants::format_first_only);

	/* Set up a buffer to receive the response */
	buffer b(2048);
	
	/* Perform the HTTP request */
	perform_query(formattedUrl,b);

	/* Decode the returned JSON */
	return parse_response(b);
	
    }

    std::string fetch_test_gibberish()
    {
	/* Set up a buffer for the fake the response */
	buffer b(2048);

	/* Sleep to simulate the HTTP request */
	std::this_thread::sleep_for( std::chrono::milliseconds(500) );

	/* Copy the fake response into the buffer */
	b.append( notfound_response.c_str(), notfound_response.length()+1 );

	/* Decode the JSON */
	return parse_response(b);

    }

    std::string fetch_test_normal()
    {
	std::this_thread::sleep_for( std::chrono::milliseconds(500) );
	return "99.99";
    }

    void fetch_test_hanging()
    {
	while (true)
	    std::this_thread::sleep_for( std::chrono::milliseconds(500) );
    }

    std::string despatch(stock_task_params_t p)
    {
	switch(p.behavior)
	{
	case SLTBNormalRequest:
	    return fetch_test_normal();
	case SLTBHangingRequest:
	    fetch_test_hanging();
	case SLTBGibberishRequest:
	    return fetch_test_gibberish();
	case SLTBNone:
	default:
	    return fetch(p.ticker);
	}

    }

    problem<stock_task_params_t,string>* make_problem(string ticker, sl_test_behavior_t b)
    {
	stock_task_params_t p;
	p.ticker = ticker;
	p.behavior = b;
	return new problem<stock_task_params_t,string>(&despatch,p);
    }


}

stock_task::stock_task(string ticker,sl_test_behavior_t b) : 
    task<stock_task_params_t,string>(make_problem(ticker,b))
{
}

void stock_task::set_completion_callback( callback* c, void* data )
{
    auto lock = state.obtain_lock();

    _callback_fn = function<callback>(c);
    _callback_data = data;
    
    state.set_entry_function( TaskState::Finished,
			      [this]() { this->notify_callback(); } );

    if (state.get_state()==TaskState::Finished)
	notify_callback();
}

void stock_task::notify_callback()
{
    _callback_fn(this,_callback_data);
}

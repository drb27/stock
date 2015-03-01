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
#include <curl/curl.h>
#include <jansson.h>
#include "buffer.h"
#include "sweepup.h"
#include "deathrattle.h"

#include "stock-task.h"

using std::string;
using std::regex;

namespace
{
    const std::string g_ticker="AAPL";

    const std::string cURL = "https://query.yahooapis.com/v1/public/yql?q=select%20Name,LastTradePriceOnly%20from%20yahoo.finance.quotes%20where%20symbol%20%3D%22{STOCK}%22&format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys&callback=";

    size_t rx_data(void *rx_buffer, size_t size, size_t nmemb, void *local_buffer)
    {
	buffer* pBuffer = reinterpret_cast<buffer*>(local_buffer);
	pBuffer->append(rx_buffer,nmemb);
	return size;
    }

    std::string fetch(const string ticker)
    {

	/* Format the URL */
	regex stock_exp("\\{STOCK\\}"); 
	auto formattedUrl = std::regex_replace(cURL,stock_exp,ticker, std::regex_constants::format_first_only);
	
	/* Get a handle to the easy curl interface, and load the URL */
	auto handle = curl_easy_init();
	curl_easy_setopt(handle, CURLOPT_URL, formattedUrl.c_str());

	/* Set up a buffer to receive the response */
	buffer b(2048);
	
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
	
	/* Decode the data */
	json_error_t error;
	json_t* root = json_loads(b.contents(), 0, &error );

	bool success=false;
	std::string output("");

	if (root)
	{
	    /* Successful parse. Continue processing. */
	    
	    sweepup<json_t*> trash( [](json_t* obj) { json_decref(obj); }  );

	    auto query = json_object_get(root,"query");
	    auto results = json_object_get(query,"results");
	    auto quote = json_object_get(results,"quote");
	    auto bid = json_object_get(quote,"LastTradePriceOnly");
	    auto str_bid = json_string_value(bid);
	    
	    success=true;
	    output = str_bid;
	    
	    trash.add(bid);
	    trash.add(quote);
	    trash.add(results);
	    trash.add(query);
	    trash.add(root);
	}

	return output;
	
    }
    
}

stock_task::stock_task(string ticker) : 
    task<const string,string>(problem<const string,string>(&fetch,ticker.c_str()))
{
}

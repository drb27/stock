/**
 * @file
 * Implementation of the tickerproblem class. This class extends urlproblem to
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

#include <regex>
#include <functional>
#include <map>
#include <curl/curl.h>
#include <jansson.h>
#include "buffer.h"
#include "sweepup.h"
#include "deathrattle.h"
#include "tickerproblem.h"

using std::string;
using std::regex;
using std::function;
using std::map;

tickerproblem::tickerproblem(const std::string& ticker, sl_test_behavior_t b ) :
    urlproblem(_url_template), _behavior(b), _ticker(ticker)
{
}

void tickerproblem::fetch(buffer& b, const std::string& url)
{
    static const char trm = '\0';

    switch (_behavior)
    {
    
    case SLTBNormalRequest:
	b.append(_fake_response.c_str(), _fake_response.length());
	b.append(&trm,1);
	break;

    case SLTBGibberishRequest:
	b.append(_notfound_response.c_str(), _notfound_response.length());
	b.append(&trm,1);
	break;

    case SLTBNone:
    default:
	urlproblem::fetch(b,url);
    }

}

map<string,string> tickerproblem::decode_response(const std::string& response)
{
    map<string,string> d;

    /* Ensures all memory is freed correctly, even if an exception is thrown */    
    sweepup<json_t*> trash( [](json_t* obj) { json_decref(obj); }  );

    /* Decode the data */
    json_error_t error;
    json_t* root = json_loads(response.c_str(), 0, &error );

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
		    auto cname = json_object_get(quote,"Name");
			
		    if ( cname && json_is_string(cname) )
		    {
			auto str_cname = json_string_value(cname);
	    
			d["companyname"] = str_cname;
		    }

		    auto bid = json_object_get(quote,"LastTradePriceOnly");
			
		    if ( bid && json_is_string(bid) )
		    {
			auto str_bid = json_string_value(bid);
	    
			d["response"] = str_bid;
			return d;
		    }
		}
	    }
	}
    }

    throw std::logic_error("No stock result found in response");
}

std::string tickerproblem::preprocess_url(const std::string& url)
{
    /* Format the URL */
    regex stock_exp("\\{STOCK\\}"); 
    auto formattedUrl = std::regex_replace(url, stock_exp, _ticker, 
					   std::regex_constants::format_first_only);

    return formattedUrl;
}

const std::string tickerproblem::_url_template = "https://query.yahooapis.com/v1/public/yql?q=select%20Name,LastTradePriceOnly%20from%20yahoo.finance.quotes%20where%20symbol%20%3D%22{STOCK}%22&format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys&callback=";

const std::string tickerproblem::_notfound_response = 
    "{\"query\":{\"count\":0,\"created\":\"2015-03-06T11:53:00Z\", \
    \"lang\":\"en-US\",\"results\":null}}";

const std::string tickerproblem::_fake_response = 
    "{\"query\":{\"count\":1,\"created\":\"2015-03-09T21:03:53Z\",\"lang\":\"en-US\", \
    \"results\":{\"quote\":{\"LastTradePriceOnly\":\"99.99\",\"Name\":\"Test Inc.\"}}}}";

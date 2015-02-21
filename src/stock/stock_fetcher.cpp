#include <iostream>
#include <regex>

using std::cout;
using std::endl;
using std::regex;

#include <curl/curl.h>
#include <jansson.h>
#include "buffer.h"
#include "sweepup.h"
#include "deathrattle.h"
#include "worker_base.h"
#include "stock_fetcher.h"

extern std::string g_ticker;

const std::string cURL = "https://query.yahooapis.com/v1/public/yql?q=select%20Name,LastTradePriceOnly%20from%20yahoo.finance.quotes%20where%20symbol%20%3D%22{STOCK}%22&format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys&callback=";

size_t stock_fetcher::rx_data(void *rx_buffer, size_t size, size_t nmemb, void *local_buffer)
{
    buffer* pBuffer = reinterpret_cast<buffer*>(local_buffer);
    pBuffer->append(rx_buffer,nmemb);
    return size;
}

void stock_fetcher::do_work(int id)
{
    /* Thread initialisation */
    this->id = id;
    log("Stock Fetcher Alive!");
    deathrattle d([this](){ log("Dead!"); } );

    /* Format the URL */
    regex stock_exp("\\{STOCK\\}"); 
    auto formattedUrl = std::regex_replace(cURL,stock_exp,g_ticker, std::regex_constants::format_first_only);

    /* Get a handle to the easy curl interface, and load the URL */
    auto handle = curl_easy_init();
    //log(formattedUrl);
    curl_easy_setopt(handle, CURLOPT_URL, formattedUrl.c_str());

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

    /* Log the data */
    //log( b.contents() );

    /* Decode the data */
    json_error_t error;
    json_t* root = json_loads(b.contents(), 0, &error );

    if (!root)
    {
	log("Error!");
    }
    else
    {
	/* Successful parse. Continue processing. */

	sweepup<json_t*> trash( [](json_t* obj) { json_decref(obj); }  );
	auto query = json_object_get(root,"query");
	auto results = json_object_get(query,"results");
	if (results) log("Got results");
	auto quote = json_object_get(results,"quote");
	auto bid = json_object_get(quote,"LastTradePriceOnly");
	auto str_bid = json_string_value(bid);
	log(str_bid);
	deliver_result(str_bid);

	trash.add(bid);
	trash.add(quote);
	trash.add(results);
	trash.add(query);
	trash.add(root);
    }

}

void stock_fetcher::deliver_result(const std::string& s )
{
    if (hasResultCallback)
	result_callback(s);
}

void stock_fetcher::set_result_callback( std::function<void(std::string)> f )
{
    result_callback = f;
    hasResultCallback = true;
}

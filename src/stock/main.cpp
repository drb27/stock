/**
 * @file main.cpp
 *
 * This is where all the magic happens
 */

#include <config.h>
#include <iostream>
#include <string>
#include <thread>
#include <list>

#include <curl/curl.h>
#include "worker_base.h"
#include "stock_fetcher.h"

using std::cout;
using std::endl;
using std::string;
using std::ostream;
using std::thread;
using std::list;

std::string g_ticker="AAPL";

int main( int argc, char* argv[] )
{
    /* Extract symbol */
    if (argc > 1 )
	g_ticker = argv[1];

    /* Initialise the CURL library */
    curl_global_init(CURL_GLOBAL_ALL);
    
    /* Create n threads, add to join list */
    list<thread> threadset;
    
    /* Create a stock fetcher thread */
    stock_fetcher s;

    s.set_result_callback( [](string s)
			   {
			       cout << s << endl;
			   }
			  );

    thread t(s,99);
    threadset.push_back(std::move(t));
    
    /* Wait for all threads to complete */
    for ( auto& t : threadset )
    {
	t.join();
    }

    /* Done! */
    return 0;
}

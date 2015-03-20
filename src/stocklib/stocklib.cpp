/**
 * @file
 * Implements the stocklib API
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

#include <exception>
#include <stdexcept>
#include <set>
#include <mutex>
#include <map>

#include <stdio.h>
#include <string.h>

#include "stocklib_p.h"
#include "tickerproblem.h"
#include "black-scholes.h"

typedef std::set<urltask*> taskset;

#define MLOCK std::lock_guard<std::recursive_mutex> lock(g_mutex)

namespace 
{
    BOOL g_initialized{false};
    sl_test_behavior_t g_behavior{SLTBNone};
    BOOL g_testmode{false};
    taskset g_taskset;
    std::recursive_mutex g_mutex;
    std::map<std::string,std::string> g_namecache;
}

inline void init_guard()
{
    if (!g_initialized)
	throw std::logic_error("stocklib must be initialized");
}

void stocklib_init()
{
    MLOCK;
    if (g_initialized)
	throw std::logic_error("stocklib is already initialized");
    
    g_initialized = true;
    g_behavior = SLTBNone;
    g_testmode = false;
    g_namecache.clear();
    g_taskset.clear();
}

void stocklib_p_reset()
{
    MLOCK;
    g_initialized=false;
    g_taskset.clear();
    g_testmode = false;
    g_behavior = SLTBNone;
    g_namecache.clear();
}

void stocklib_p_test_mode(BOOL enable)
{
    MLOCK;
    init_guard();

    g_testmode = enable;
}

void stocklib_p_test_behavior( sl_test_behavior_t b )
{
    MLOCK;
    init_guard();

    g_behavior = b;
}

int stocklib_p_open_handles()
{
    MLOCK;
    init_guard();

    return g_taskset.size();
}

SLHANDLE stocklib_fetch_asynch(const char* ticker, char* output)
{
    MLOCK;
    init_guard();

    // Create a problem
    tickerproblem* pProblem = new tickerproblem(ticker,(g_testmode)?g_behavior:SLTBNone);

    // Create a urltask
    urltask* pNewTask = new urltask(pProblem);
    
    g_taskset.insert(pNewTask);

    pNewTask->perform_async( [=]()
			     {
				 strcpy(output, pNewTask->output()["response"].c_str() );
				 g_namecache[ticker] = pNewTask->output()["companyname"];
			     } );
    return pNewTask;
}

void stocklib_asynch_dispose(SLHANDLE h)
{
    MLOCK;
    init_guard();

    // Is this a known task?
    if ( g_taskset.find(h)!=g_taskset.end())
    {
	// Yes - now check it is in a disposable state
	if ( h->ready() )
	{
	    auto l = h->obtain_lock();
	    l.unlock();
	    l.release();
	    g_taskset.erase(h);
	    delete h;
	}
	else
	    throw std::logic_error("Can't dispose of this handle - async request in progress");
    }
    else
	throw std::logic_error("Invalid handle");
}

sl_result_t stocklib_fetch_synch(const char* ticker, char* output)
{
    MLOCK;
    init_guard();

    // Create a problem
    tickerproblem* pProblem = new tickerproblem(ticker,(g_testmode)?g_behavior:SLTBNone);

    // Create a task on the stack for immediate execution
    urltask t(pProblem);

    WorkResult r = t.perform_sync();
    if (r==WorkResult::Success)
    {
	strcpy(output, t.output()["response"].c_str() );
	g_namecache[ticker] = t.output()["companyname"];
	return SL_OK;
    }
    else
	return SL_FAIL;
}

BOOL stocklib_is_complete( SLHANDLE h )
{
    MLOCK;
    init_guard();

    // Is this a known task?
    if ( g_taskset.find(h)!=g_taskset.end() )
    {
	return h->ready();
    }
    else
	throw std::logic_error("Invalid handle");
}

sl_result_t stocklib_asynch_wait( SLHANDLE h, int timeout)
{
    MLOCK;
    init_guard();

    // Is this a known task?
    if ( g_taskset.find(h)!=g_taskset.end() )
    {
	WorkResult r = h->wait();
	if (r==WorkResult::Success) 
	    return SL_OK;
	else
	    return SL_FAIL;
	
    }
    else
	throw std::logic_error("Invalid handle");
    
}

sl_result_t stocklib_asynch_register_callback(SLHANDLE h, SLCALLBACK c, void* data)
{
    MLOCK;
    init_guard();

    // Is this a known task?
    if ( g_taskset.find(h)!=g_taskset.end() )
    {
	h->set_completion_callback(c,data);
	return SL_OK;
    }
    else
	return SL_FAIL;
}

sl_result_t stocklib_asynch_result(SLHANDLE h)
{
    MLOCK;
    init_guard();

    // Is this a known task?
    if ( g_taskset.find(h)!=g_taskset.end() )
    {
	if ( h->ready() )
	{
	    if ( h->result() == WorkResult::Success )
		return SL_OK;
	    else
		return SL_FAIL;
	}
	else
	    return SL_PENDING;
    }
    else
	throw std::logic_error("Invalid handle");
}

sl_result_t stocklib_wait_all()
{
    MLOCK;
    init_guard();

    // Wait for all tasks to complete
    for ( auto h : g_taskset )
    {
	// Wait for entry into the finish state
	if (!h->ready())
	    h->wait();

	// Wait for state entry actions to complete
	auto l = h->obtain_lock();
	l.unlock();
	l.release();

    }

    return SL_OK;

}

sl_result_t stocklib_cleanup()
{
    MLOCK;
    init_guard();

    for ( auto h : g_taskset )
    {
	if (h->ready())
	    delete h;
	else
	    return SL_FAIL;
    }

    g_taskset.clear();
    return SL_OK;

}

BOOL stocklib_p_namecache_has_ticker(const char* ticker)
{
    MLOCK;
    return g_namecache.find(ticker)!=g_namecache.end();
}

int stocklib_p_namecache_count()
{
    MLOCK;
    return g_namecache.size();
}

const char* stocklib_p_namecache_resolve(const char* ticker)
{
    MLOCK;
    static thread_local char buffer[256];
    
    if (g_namecache.count(ticker))
    {
	strcpy(buffer,g_namecache[ticker].c_str());
	return buffer;
    }
    else
	return NULL;
}

const char* stocklib_p_namecache_insert( const char* ticker, const char* name )
{
    MLOCK;
    g_namecache[ticker] = name;
}

const char* stocklib_ticker_to_name( const char* ticker )
{
    MLOCK;
    static thread_local char buffer[256];

    if (g_namecache.count(ticker))
    {
	strcpy(buffer,g_namecache[ticker].c_str());
	return buffer;
    }
    else
    {
	if ( SL_OK == stocklib_fetch_synch(ticker,buffer) )
	{
	    strcpy(buffer,g_namecache[ticker].c_str());
	    return buffer;
	}
	else
	    return NULL;
    }
}

/*
double bs_call_price(double asset_price, double strike_price, double expiry, double r,
		     double vol);
*/
double stocklib_option_price(option_params_t params)
{
    return bs_call_price(params.asset_price, params.strike_price,params.expiry,
		  params.interest, params.volatility);
}

extern double stocklib_option_greek(const option_params_t params, option_greek_t greek)
{
    switch (greek)
    {
    case SLGDelta:
	if (params.otype == SLOTCall)
	    return bs_call_delta(params.asset_price,params.strike_price,params.expiry,
				 params.interest,params.volatility);
	else
	    return bs_put_delta(params.asset_price,params.strike_price,params.expiry,
				 params.interest,params.volatility);

    default:
	return 0.0;
    };
}

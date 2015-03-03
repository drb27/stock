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

#include <stdio.h>
#include <string.h>

#include "stocklib_p.h"
#include "stock-task.h"

typedef std::set<stock_task*> taskset;

#define MLOCK std::lock_guard<std::mutex> lock(g_mutex)

enum class OperatingMode
{
    Normal,
	Test
};

namespace 
{
    BOOL g_initialized{false};
    OperatingMode g_mode{OperatingMode::Normal};
    taskset g_taskset;
    std::mutex g_mutex;
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
    g_initialized=true;
}

void stocklib_p_test_mode()
{
    MLOCK;
    init_guard();

    g_mode = OperatingMode::Test;
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
    
    stock_task* pNewTask = new stock_task(ticker);
    g_taskset.insert(pNewTask);

    pNewTask->perform_async();
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
    stock_task t(ticker);
    t.perform_sync();
    strcpy(output, t.output().c_str() );
    return SL_OK;
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

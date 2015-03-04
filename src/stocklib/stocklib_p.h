/**
 * @file 
 * Private header for the stocklib library API. The private header defines
 * types and API calls which are only available to the implementation, and to
 * unit tests.
 * 
 * To have access to the full API, including the private API, include this
 * header and do not manually include stocklib.h. Note that some types will be
 * defined differently (e.g. SLHANDLE), unless you #define STOCKLIB_P_H_C. If
 * you do that, the header will be C compatible too - otherwise, C++ only,
 * please.
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

#ifndef STOCKLIB_P_H
#define STOCKLIB_P_H
#define SL_HAS_PRIVATE

#ifndef STOCKLIB_P_H_C
class stock_task;
typedef stock_task* SLHANDLE;
#else
typedef void* SLHANDLE
#endif

#include "stocklib.h"
#include <stocklib/stock-task-modes.h>

#ifdef STOCKLIB_P_H_C
extern "C" {
#endif

/**
 * Enables or disables test mode. The behavior mode set by the most recent call
 * to stocklib_p_test_behavior is preserved, but is only effective when test
 * mode is enabled. If test mode is enabled before any call to set the test
 * behavior, then SLNormalRequest is the default behavior.
 *
 * @param enable True to enable test mode, false otherwise. 
 */
extern void stocklib_p_test_mode(BOOL enable);

/**
 * Sets the behavior to be used for all subsequent synchronous or asynchronous
 * requests performed when test mode is enabled. The mode survives any call to
 * stocklib_p_test_mode().
 *
 * @param b The test behavior to exhibit during test mode
 */
extern void stocklib_p_test_behavior( sl_test_behavior_t b);

/**
 * Queries the number of open (undisposed) handles
 *
 * @return the number of open handles
 */
extern int  stocklib_p_open_handles();
    
/**
 * Performs a hard reset of the library. If handles are open,
 * they are not cleaned up (resulting in memory leaks), and
 * if in progress they may run to completion. Used to allow
 * re-initialization for testing purposes.
 */    
extern void stocklib_p_reset();

#ifdef STOCKLIB_P_H_C
}
#endif

#endif

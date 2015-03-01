/**
 * @file 
 * This is the public API to the stocklib library. It should be 
 * included by any translation unit wanting to use the API. 
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

#ifndef STOCKLIB_H
#define STOCKLIB_H

/**
 * The maximum number of bytes ever writen to a program buffer
 */
#define SL_MAX_BUFFER (32)

/**
 * Enumeration with possible return codes from the library API.
 */
typedef enum 
{
    SL_OK=0,			/**< The request succeeded   */
    SL_FAIL=1,			/**< The request failed      */
    SL_PENDING=2,		/**< The request is pending  */
    SL_TIMEOUT=3		/**< The request timed out   */
} sl_result_t;


#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Partial opaque type for communicating between the library and the
     * program.
     */
    typedef void *SLHANDLE;


    /**
     * Initializes the library. Must be called exactly once per run of the 
     * program, and before any other call to this library. 
     *
     * @note No cleanup call is required, but all calls to 
     * stocklib_fetch_asynch() should be balanced with calls to
     * stocklib_asynch_dispose() to avoid memory leaks.  
     */
    extern stocklib_init();


    /**
     * Synchronously fetches the latest trade price of a stock. 
     *
     * If the function returns SL_OK, then the buffer will have been
     * populated with the latest trade price of the stock identified by 
     * the ticker parameter. If the function fails, the buffer is 
     * untouched. 
     *
     * @param ticker the ticker symbol for the stock
     * @param output a program-owned buffer where the output will be written.
     * @return a result code indicating the outcome of the request
     *
     * @note You must pass a buffer of at least SL_MAX_BUFFER bytes. 
     */
    extern sl_result_t stocklib_fetch_synch( const char* ticker, char* output );


    /**
     * Begins the asynchronous fetching of the latest trade price of a stock. 
     * The call returns immediately. The output is not available until the
     * asychronous operation has been completed (check with stocklib_is_complete). 
     *
     * @warning Each call to this function must be matched with a call to
     *          stocklib_asynch_dispose(), otherwise a memory leak will occur. 
     *
     * @param ticker the ticker symbol for the stock
     * @param output a program-owned buffer where the output will be written.
     * @return a handle which can be used to identify this particular transaction. 
     */
    extern SLHANDLE stocklib_fetch_asynch( const char* ticker, char* output );

    /**
     * Clears up the memory allocated during a call be stocklib_fetch_asynch().
     *
     * @param h a handle to a valid asynchronous operation, which has not yet
     *          been disposed of.  
     *
     * @warning do not pass a handle to any other API call after you have
     *          disposed of it. A core dump is the likely outcome.
     */
    extern void stocklib_asynch_dispose( SLHANDLE h );


    /**
     * Queries whether or not the given operation has completed. Note that this
     * tells you nothing about whether or not the operation was successful, or
     * encountered an error. 
     *
     * @param h a handle to a valid asynchronous operation, which has not yet been
     *          disposed of via stocklib_asynch_dispose(). 
     *
     * @return true if the operation has completed, false otherwise. 
     */
    extern BOOL stocklib_is_complete( SLHANDLE h );


    /**
     * Gets the result code for a completed asynchronous operation. The caller
     * should ensure that the operation has been completed by calling
     * stocklib_is_complete() prior to this function. If the operation is ongoing,
     * this function immediately returns SL_PENDING.
     *
     * @param h a handle to a valid asynchronous operation, which has not yet been
     *          disposed of via stocklib_asynch_dispose().
     */
    extern sl_result_t stocklib_asynch_result( SLHANDLE h );


    /**
     * Waits for a valid asynchronous operation to complete, then returns the
     * result code. If the operation had already completed, this function returns
     * the correct result code immediately. If a timeout is specified, and the 
     * operation doesn't complete in time, SL_TIMEOUT is returned and the operation
     * continues in the background. 
     * 
     * @param h a handle to a valid asynchronous operation, which has not yet been
     *          disposed of via stocklib_asynch_dispose().
     * @param timeout The number of milliseconds to wait. 0 means forever. 
     *
     * @return the outcome of the asynchronous operation
     */
    extern sl_result_t stocklib_asynch_wait( SLHANDLE h, int timeout=0 );

#ifdef __cplusplus
}
#endif

#endif

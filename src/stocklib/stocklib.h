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

#include <config.h>

typedef char BOOL;

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

typedef enum _option_type_t 
{
    SLOTCall,
    SLOTPut
} option_type_t;

typedef enum _option_greek_t
{
    SLGDelta,
    SLGTheta,
    SLGVega
} option_greek_t;

/**
 * Structure representing the parameters needed to estimate the price of an option.
 */
typedef struct _option_params_t
{
    option_type_t otype;	/**< Call or put  */
    double asset_price;		/**< Price of the underlying  */
    double strike_price;	/**< Strike price of the option  */
    double volatility;		/**< Volatility of the underlying, 1.00 = 100%  */
    double interest;		/**< Risk-free interest rate, 1.00 = 100%  */
    double expiry;		/**< Years to expiry, 1.00 = 365 days  */
} option_params_t;

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SL_HAS_PRIVATE
    /**
     * Partial opaque type for communicating between the library and the
     * program.
     */
    typedef void *SLHANDLE;
#endif

    /**
     * Type definition for callback functions
     *
     * @param h A handle to the asynchronous request this callback was triggered by
     * @param data an application-defined pointer to some data
     */
    typedef void (*SLCALLBACK)(SLHANDLE h,void* data);


    /**
     * Initializes the library. Must be called exactly once per run of the 
     * program, and before any other call to this library. 
     *
     * @note No cleanup call is required, but all calls to 
     * stocklib_fetch_asynch() should be balanced with calls to
     * stocklib_asynch_dispose() to avoid memory leaks.
     *
     * @warning You MUST initialize the curl library with a call to
     * curl_global_init() before using the stocklib library!  
     */
    extern void stocklib_init();

    /**
     * Returns the full name of the security represeted by the ticker symbol
     * provided. 
     *
     * @note The first time this call is made with a given symbol, and internet
     * lookup is performed. The result is then cached. Subsequent calls with the
     * same ticker symbol will return the cached result. Cache entries do not
     * expire, and do not survive calls to stocklib_cleanup(). The cache is
     * memory based and is therefore clear on each run of the application.  
     *
     * @param ticker The ticker symbol to resolve to a name
     * @return The name of the underlying security, or NULL if a problem
     * occured.
     */
    extern const char* stocklib_ticker_to_name( const char* ticker );

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

    /**
     * Registers a callback which will be called upon completion of an asychronous
     * operation. If the operation has already completed, the callback will be called
     * immediately. 
     *
     * @param h    A handle to the operation on which to set the callback
     * @param c    A pointer to the callback function
     * @param data An application defined data pointer which will be passed to c
     *
     * @return SL_OK if the callback was set, SL_FAIL otherwise.
     */
    extern sl_result_t stocklib_asynch_register_callback(SLHANDLE h, SLCALLBACK c, void* data);

    /**
     * Waits for all pending operations to complete.
     */
    extern sl_result_t stocklib_wait_all();

    /**
     * Disposes of any remaining open handles, regardless of state. 
     * Call stocklib_wait_all() first, if you want to ensure no 
     * operations are pending. 
     */
    extern sl_result_t stocklib_cleanup();

    /**
     * Calculates the theoretical value of a option contract from the parameters
     * passed in. Uses the Black-Scholes method. 
     */
    extern double stocklib_option_price(const option_params_t params);

    /**
     * Calculates one of the greek values of an option from the parameters
     * passed in.
     *
     * @param params The set of parameters describing the option
     * @param greek The greek value to calculate
     *
     * @return The value of the greek requested for the option.
     */
    extern double stocklib_option_greek(const option_params_t params, option_greek_t greek);

#ifdef __cplusplus
}
#endif

#endif

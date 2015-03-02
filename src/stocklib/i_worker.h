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

#ifndef I_WORKER_H
#define I_WORKER_H

#include "i_resultor.h"

/**
 * Result code for i_worker implementations.
 */
enum class WorkResult
{
    Unknown=0,			///< The work has not been done yet
	Success,		///< The work completed successfuly
	Failure			///< The work was attempted but was unsuccessful
	};

/**
 * Interface definition for a class which can perform an action 
 * either synchronously, or asynchronously.
 */
template<class T,class E>
class i_worker : public i_resultor<WorkResult,E>
{
    
public:

    /**
     * Default constructor. 
     */
    i_worker() : i_resultor<WorkResult,E>(WorkResult::Unknown) {}
    
    /**
     * Performs the work synchronously, and returns a result code when done.
     * @return A WorkResult code indicating the degree of success with which the operation completed.
     */
    virtual WorkResult perform_sync() =0;

    /**
     * Requests that the work be performed asynchronously. Call wait() to block the calling
     * thread until the result is available. 
     */ 
    virtual void perform_async() =0;

    /**
     * Waits for the work to complete, and returns the result code.
     * If the work is not even started, std::logic_error is raised. If the work has
     * already completed, the result code is immediately returned.  
     */
    virtual WorkResult wait() const =0;
    
};

#endif

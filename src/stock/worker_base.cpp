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

#include <iostream>
#include <thread>
#include <mutex>

using std::mutex;
using std::lock_guard;
using std::cout;
using std::endl;

#include "state.h"
#include "worker_base.h"

/**
 * Basic constructor. 
 * Prepares the object, but does not invoke the worker
 * functionality. 
 */
worker_base::worker_base()
{
}

/**
 * Logs a string message to the given stream. 
 *
 * Using cout as a default, formats a log message and writes it
 * to the desired stream. Includes the worker id in the log message
 * so the reader can tell which worker generated the message. 
 *
 * @param msg the raw message to log
 * @param os the output stream to use (defaults to std::cout)
 */
void worker_base::log(const string& msg, ostream& os) const
{
    /*
    static mutex cs;
    {
	lock_guard<mutex> l(cs);
	os << "[WRK" << id << "]: " << msg << endl;
	}*/
}

/**
 * Sets the action to be completed when the worker's job is done.
 *
 * @param f A function object, functor, or function pointer implementing the action
 */
void worker_base::set_completion_action( function<void()> f )
{
    hasCompletionAction = true;
    completion = f;
}

/**
 * Allows the object to be passed directly to a thread object.
 * This method invokes the do_work() function, which contains the logic for the
 * worker thread.
 *
 * @param i An integer that the thread uses to identify itself in the log.
 */
void worker_base::operator()(int i)
{
    try
    {
	do_work(i);
    }
    catch (std::exception) {}
    if (hasCompletionAction)
	completion();
}

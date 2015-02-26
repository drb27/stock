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

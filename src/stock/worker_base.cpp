#include <iostream>
#include <thread>
#include <mutex>

using std::mutex;
using std::lock_guard;
using std::cout;
using std::endl;

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
    static mutex cs;
    {
	lock_guard<mutex> l(cs);
	os << "[WRK" << id << "]: " << msg << endl;
    }
}

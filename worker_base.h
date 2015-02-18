#ifndef WORKER_BASE_H
#define WORKER_BASE_H

#include <string>
#include <ostream>

using std::string;
using std::ostream;

/**
 * Simple implemetation of a worker thread.
 *
 * Designed to be used with C++11 threads. This is an abstract class,
 * you should inherit from it and implement operator()(int) to provide
 * the functionality of the worker. Use log() to write logging and/or
 * debugging messages.
 *
 * To create and immediately start a thread, pass an instance of a
 * worker_base derived object to std::thread, along with a worker id
 * to use:
 * @code
 *     my_worker w;
 *     std::thread t(w,1);
 * @endcode 
 */
class worker_base
{
public:

    worker_base();
    virtual void operator()(int)=0;

protected:
    virtual void log(const string& msg, ostream& os = std::cout ) const final;
    int id;
};

#endif

#ifndef WORKER_BASE_H
#define WORKER_BASE_H

#include <string>
#include <ostream>
#include <functional>
#include <thread>
#include "problem.h"

#include "i_resultor.h"
#include "i_worker.h"

using std::string;
using std::ostream;
using std::function;

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
    virtual void operator()(int) final;

    /**
     * Implements the work of the worker. Abstract in worker_base, classes inheriting
     * this method must implement it if instances are to be realized at runtime.
     */

    virtual void do_work(int)=0;
    
    virtual void set_completion_action( function<void()> f ) final;
protected:
    virtual void log(const string& msg, ostream& os = std::cout ) const final;
    int id;
    bool hasCompletionAction = false;
    function<void()> completion;
};

#endif

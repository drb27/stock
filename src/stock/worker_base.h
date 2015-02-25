#ifndef WORKER_BASE_H
#define WORKER_BASE_H

#include <string>
#include <ostream>
#include <functional>

using std::string;
using std::ostream;
using std::function;

/**
 * Interface definition for a class which can perform an action 
 * either synchronously, or asynchronously.
 */
template<class T>
class i_worker
{

public:

virtual T perform_sync() =0;
virtual void perform_async() =0;
virtual bool has_performed() const =0;
virtual T retrieve_async() const =0;
virtual T wait() const =0;

};

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
    virtual void do_work(int)=0;
    
    virtual void set_completion_action( function<void()> f ) final;
protected:
    virtual void log(const string& msg, ostream& os = std::cout ) const final;
    int id;
    bool hasCompletionAction = false;
    function<void()> completion;
};

#endif

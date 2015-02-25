#ifndef WORKER_BASE_H
#define WORKER_BASE_H

#include <string>
#include <ostream>
#include <functional>
#include <thread>
#include <type_traits>

using std::string;
using std::ostream;
using std::function;

enum class WorkResult
{
    NotPerformed=0,
	Success,
	Failure
	};

/**
 * Interface definition for a class which can return a result.
 * The result type is parameterized.
 * 
 * @param R The type of the result to be returned.
 *
 * @note The default value of the result immediately after construction is zero,
 *       regardless of whether or not this is a valid  
 */
template<class R>
class i_resultor
{
    static_assert(std::is_enum<R>::value,"i_resultor can only be used with enumerations");

public:
    /**
     * Returns a reference to the current result.
     */
    const R& result() const { return _result; }
    
    /**
     * Returns a reference to the current exception.
     * Typically only useful if the result code indicates a failure
     */
    const std::exception& error() const { return _exception; }

protected:
    R _result = R();
    std::exception _exception = std::exception();
};

/**
 * Interface definition for a class which can perform an action 
 * either synchronously, or asynchronously.
 */
template<class T>
class i_worker : public i_resultor<WorkResult>
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

#ifndef WORKER_BASE_H
#define WORKER_BASE_H

#include <string>
#include <ostream>
#include <functional>
#include <thread>
#include <type_traits>
#include "problem.h"

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
    
    /**
     * Performs the work synchronously, and returns a result code when done.
     * @return A WorkResult code indicating the degree of success with which the operation completed.
     */
    virtual WorkResult perform_sync() =0;
    virtual void perform_async() =0;
    virtual bool has_performed() const =0;
    virtual T& retrieve() const =0;
    virtual WorkResult wait() const =0;
    
};

/** 
 * A class which performs a task, either synchronously or
 * asynchronously.
 */
template<class Ti, class To>
class task : public i_worker<To>
{
public:

    task( problem<Ti,To>& p ) : _problem(p) {}

    ///@name implementation of i_worker:
    ///@{

    virtual WorkResult perform_sync()
	{
	    WorkResult returnCode = WorkResult::NotPerformed;

	    try
	    {
		_output = _problem();
		returnCode = WorkResult::Success;
	    }
	    catch ( const std::exception& e )
	    {
		this->_exception = e;
		return WorkResult::Failure;
	    }

	    this->_result = returnCode;
	    _performed=true;
	    return returnCode;

	}

    virtual bool has_performed() const
	{
	    return _performed;
	}

    virtual To& retrieve() const
	{
	    return *_output;
	}
    

    ///@}

protected:
    problem<Ti,To>  _problem;
    bool _performed=false;
    To* _output = nullptr;
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

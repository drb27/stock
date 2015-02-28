#ifndef TASK_H
#define TASK_H

#include <functional>
#include <exception>
#include <thread>

#include "i_worker.h"
#include "problem.h"

/** 
 * A class which performs a task, either synchronously or
 * asynchronously.
 */
template<class Ti, class To>
class task : public i_worker<To>
{
public:

    task( problem<Ti,To>& p, std::function<void()>& c  ) : _problem(p), _completion(c) {}

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

    virtual void perform_async()
	{
	    _thread = new std::thread( [&] () 
				       {
					   this->perform_sync();
					   delete this->_thread;
					   this->_thread = nullptr;
				       } );
	}

    virtual WorkResult wait() const
	{
	    if (_thread)
	    {
		_thread->join();
		return WorkResult::NotPerformed;
	    }
	    else
	    {
		return this->_result;
	    }
	} 
    

    ///@}

protected:
    const problem<Ti,To>  _problem;
    const std::function<void()> _completion;
    bool _performed=false;
    To* _output = nullptr;
    std::thread* _thread = nullptr;
}; 

#endif

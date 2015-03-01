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

#ifndef TASK_H
#define TASK_H

#include <iostream>
#include <functional>
#include <exception>
#include <thread>
#include <mutex>
#include <memory>

#include "i_resultor.h"
#include "i_worker.h"
#include "problem.h"
#include "state.h"



/** 
 * A class which performs a task, either synchronously or
 * asynchronously.
 */
template<class Ti, class To>
class task : public i_worker<To,typename ::problem<Ti,To>::abort_exception>
{
public:
    
    typedef typename problem<Ti,To>::abort_exception extype;
    
    /**
     * Possible states that a task object can be in.
     */
    enum class TaskState
    {
	NotPerformed,		///< The task pending / ready to run
	    InProgress,		///< The task is running
	    Finished		///< The task has finished, a result code is available
	    };

    /**
     * Possible actions on the internal state machine
     */
    enum class TaskAction
    {
	Begin,			///< Start the task
	    Abort,		///< Abort a running task
	    Finish,		///< Complete a task
	    Reset		///< Prepare the object for re-use
    };
    
    task( problem<Ti,To>& p) : _problem(p) 
    {
	// Add states and actions
	state.add_states( { TaskState::NotPerformed, 
		    TaskState::InProgress,
		    TaskState::Finished } );

	state.add_actions( { TaskAction::Begin,
		    TaskAction::Abort,
		    TaskAction::Finish,
		    TaskAction::Reset } );
    
	// Add transitions
	state.add_transition( TaskState::NotPerformed, 
			      TaskAction::Begin, TaskState::InProgress );

	state.add_transition( TaskState::InProgress, 
			      TaskAction::Abort, TaskState::NotPerformed );

	state.add_transition( TaskState::InProgress, 
			      TaskAction::Finish, TaskState::Finished );

	state.add_transition( TaskState::Finished, 
			      TaskAction::Reset, TaskState::NotPerformed );

	// Set the initial state
	state.initialize(TaskState::NotPerformed);
    }

    ///@name implementation of i_worker:
    ///@{
    
    virtual WorkResult perform_sync()
    {
	std::lock_guard<std::mutex> guard(_mutex);
	state.action(TaskAction::Begin);
	
	perform();

	return i_worker<To,extype>::result();
    }

    virtual void perform_async()
    {
	std::lock_guard<std::mutex> guard(_mutex);
	state.action(TaskAction::Begin);

	std::thread t( [this]()
		       {
			   this->perform();
		       } );
	
	t.detach();
	
    }

    virtual WorkResult wait() const
    {
	state.wait_for_state_entry(TaskState::Finished);
	return i_worker<To,extype>::result();
    } 


    ///@}

    virtual void reset()
    {
	std::lock_guard<std::mutex> guard(_mutex);
	i_resultor<WorkResult,extype>::reset_result(WorkResult::Unknown);
	state.action(TaskAction::Reset);
    }

    virtual To output() const
    {
	std::lock_guard<std::mutex> guard(_mutex);
	return *_output;
    }

private:

    virtual void perform() final
    {
	try
	{
	    _output = std::unique_ptr<To>(new To(_problem() ));
	    i_worker<To,extype>::set_result(WorkResult::Success);	
	}
	catch ( const extype& e )
	{
	    i_worker<To,extype>::set_result(WorkResult::Failure,e);
	}
	catch ( const std::exception& e )
	{
	    i_worker<To,extype>::set_result(WorkResult::Failure,
					    extype("Unexpected exception thrown from problem execution"));
	}

	state.action(TaskAction::Finish);	
    }

protected:

    mutable std::mutex _mutex;
    const problem<Ti,To>  _problem;
    std::unique_ptr<To> _output = nullptr;
    state_machine<TaskState,TaskAction> state;
}; 

#endif

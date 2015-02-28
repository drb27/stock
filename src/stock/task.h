#ifndef TASK_H
#define TASK_H

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
class task : public i_worker<To>
{
public:

    enum class TaskState
    {
	NotPerformed,
	    InProgress,
	    Finished
	    };

    enum class TaskAction
    {
	Begin,
	    Abort,
	    Finish,
	    Reset
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

	return i_worker<To>::result();
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
	return i_worker<To>::result();
    } 


    ///@}

    virtual void reset()
    {
	std::lock_guard<std::mutex> guard(_mutex);
	i_resultor<WorkResult>::reset_result(WorkResult::Unknown);
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
	    i_worker<To>::set_result(WorkResult::Success);	
	}
	catch ( const std::exception& e )
	{
	    i_worker<To>::set_result(WorkResult::Failure,e);
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

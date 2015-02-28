#ifndef TASK_H
#define TASK_H

#include <functional>
#include <exception>
#include <thread>

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
	state.action(TaskAction::Begin);
	
	try
	{
	    _problem();
	    i_worker<To>::set_result(WorkResult::Success);	
	}
	catch ( const std::exception& e )
	{
	    i_worker<To>::set_result(WorkResult::Failure,e);
	}

	state.action(TaskAction::Finish);
	return i_worker<To>::result();
    }

    virtual void perform_async()
    {
    }

    virtual WorkResult wait() const
    {
	i_worker<To>::set_result(WorkResult::Failure);
	state.action(TaskAction::Finish);
	return i_worker<To>::result();
    } 
    

    ///@}

protected:
    const problem<Ti,To>  _problem;
    To* _output = nullptr;
    state_machine<TaskState,TaskAction> state;
}; 

#endif

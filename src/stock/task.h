#ifndef TASK_H
#define TASK_H

#include <functional>
#include <exception>
#include <thread>

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
	    Succeeded,
	    Failed
    };

    enum class TaskAction
    {
	Begin,
	    TerminateSuccessfully,
	    Abort
	    };

    task( problem<Ti,To>& p) : _problem(p) 
    {
	// Add states and actions
	state.add_states( { TaskState::NotPerformed, TaskState::InProgress,
		    TaskState::Succeeded, TaskState::Failed } );

	state.add_actions( { TaskAction::Begin, TaskAction::TerminateSuccessfully, 
		    TaskAction::Abort } );
    
	// Add transitions
	state.add_transition( TaskState::NotPerformed, TaskAction::Begin, TaskState::InProgress );
	state.add_transition( TaskState::NotPerformed, TaskAction::Abort, TaskState::Failed );
	
	state.add_transition( TaskState::InProgress, TaskAction::TerminateSuccessfully, 
			      TaskState::Succeeded );

	state.add_transition( TaskState::InProgress, TaskAction::Abort, TaskState::Failed );


	// Add entry/exit actions

	// Set the initial state
	state.initialize(TaskState::NotPerformed);
    }

    ///@name implementation of i_worker:
    ///@{
    
    virtual WorkResult perform_sync()
    {
	return WorkResult::Failure;
    }

    virtual void perform_async()
    {
    }

    virtual bool is_complete() const
    {
	return state.get_state()==TaskState::Succeeded;
    }

    virtual To& retrieve() const
    {
	return *_output;
    }

    virtual WorkResult wait() const
    {
	return WorkResult::Failure;
    } 
    

    ///@}

protected:
    const problem<Ti,To>  _problem;
    To* _output = nullptr;
    state_machine<TaskState,TaskAction> state;
}; 

#endif

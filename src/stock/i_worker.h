#ifndef I_WORKER_H
#define I_WORKER_H

#include "i_resultor.h"

enum class WorkResult
{
    NotPerformed=0,
	Success,
	Failure
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

    /**
     * Requests that the work be performed asynchronously. Call wait() to block the calling
     * thread until the result is available. 
     */ 
    virtual void perform_async() =0;

    /** 
     * True if the work was completed successfully, and a result code is available
     */
    virtual bool is_complete() const =0;

    /**
     * Retrieves the output of the work.
     * Throws std::logic_error if the result of the work is not yet available.  
     */
    virtual T& retrieve() const =0;

    /**
     * Waits for the work to complete, and returns the result code.
     * If the work is not even started, std::logic_error is raised. If the work has
     * already completed, the result code is immediately returned.  
     */
    virtual WorkResult wait() const =0;
    
};

#endif

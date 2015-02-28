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
    virtual void perform_async() =0;
    virtual bool has_performed() const =0;
    virtual T& retrieve() const =0;
    virtual WorkResult wait() const =0;
    
};

#endif

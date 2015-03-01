#include "../stock/task.h"
#include "test-task.h"

#include <thread>

#define SIX_FACTORIAL (720)
#define ITERATIONS (100)

namespace
{
    class failtest_exception : public std::logic_error 
    {
    public:
	explicit failtest_exception(const std::string& w ) : std::logic_error(w) {}
	explicit failtest_exception(const char* w) : std::logic_error(w) {}
    };

    long fct(long n)
    {
	if (n==1) return 1;
	return n*fct(n-1);
    }
    
    long fct_success(long n)
    {
	for ( int i =0; i < ITERATIONS; i++ )
	{
	    // Compute six factorial
	    int result = fct(n);
	    std::this_thread::sleep_for( std::chrono::milliseconds(1) );
	    if (result!=SIX_FACTORIAL)
		throw std::logic_error("There is a bug in fct()");
	}
	
	return fct(n);
    }

    long fct_buggy(long n)
    {
	for ( int i =0; i < ITERATIONS; i++ )
	{
	    // Compute six factorial
	    int result = fct(n);
	    if (i==ITERATIONS-2) result++;
	    std::this_thread::sleep_for( std::chrono::milliseconds(1) );
	    if (result!=SIX_FACTORIAL)
		throw failtest_exception("There is a bug in fct()");
	}
	
	return fct(n);
    }
    
}

TaskTestFixture::TaskTestFixture()
{
}

TaskTestFixture::~TaskTestFixture()
{

}

void TaskTestFixture::setUp()
{
    problem<long,long> prb(&fct_success,6);
    _task = std::unique_ptr<task<long,long>>(new task<long,long>(prb));

    problem<long,long> buggy_prb(&fct_buggy,6);
    _buggy_task = std::unique_ptr<task<long,long>>(new task<long,long>(buggy_prb));
}

void TaskTestFixture::tearDown()
{
    _task = nullptr;
}

/**
 * Tests object creation 
 */
void TaskTestFixture::testConstruction()
{
    CPPUNIT_ASSERT( !_task->ready() );
}

/**
 * Tests a normally-terminating problem synchronously 
 */
void TaskTestFixture::testPerformSyncSuccess()
{
    CPPUNIT_ASSERT( !_task->ready() );
    WorkResult r = _task->perform_sync();

    CPPUNIT_ASSERT( _task->ready() );
    CPPUNIT_ASSERT( r == WorkResult::Success );
    CPPUNIT_ASSERT( SIX_FACTORIAL == _task->output() );
    
}

/**
 * Tests a normally-terminating problem asynchronously 
 */
void TaskTestFixture::testPerformASyncSuccess()
{
    CPPUNIT_ASSERT( !_task->ready() );
    _task->perform_async();

    WorkResult r = _task->wait();

    CPPUNIT_ASSERT( _task->ready() );
    CPPUNIT_ASSERT( r == WorkResult::Success );
    CPPUNIT_ASSERT( SIX_FACTORIAL == _task->output() );
    
}

/**
 * Tests that you can't start two asynchonous operations simultaneously 
 */
void TaskTestFixture::testPerformAsyncThenTryAgain()
{
    CPPUNIT_ASSERT( !_task->ready() );
    _task->perform_async();
    CPPUNIT_ASSERT_THROW( _task->perform_async(), std::logic_error );

    WorkResult r = _task->wait();

    CPPUNIT_ASSERT( _task->ready() );
    CPPUNIT_ASSERT( r == WorkResult::Success );
    CPPUNIT_ASSERT( SIX_FACTORIAL == _task->output() );
    
}

/**
 * Tests object re-use
 */
void TaskTestFixture::testReset()
{
    testPerformSyncSuccess();
    _task->reset();
    testPerformSyncSuccess();
}

/**
 * Tests a failing problem synchronously 
 */
void TaskTestFixture::testPerformSyncFailure()
{
    using extype = problem<long,long>::abort_exception;

    CPPUNIT_ASSERT( !_buggy_task->ready() );
    WorkResult r = _buggy_task->perform_sync();

    CPPUNIT_ASSERT( _buggy_task->ready() );
    CPPUNIT_ASSERT( r == WorkResult::Failure );
    CPPUNIT_ASSERT_THROW( throw _buggy_task->exception(), extype);
    
}

/**
 * Tests a normally-terminating problem asynchronously 
 */
void TaskTestFixture::testPerformAsyncFailure()
{
    CPPUNIT_ASSERT( !_buggy_task->ready() );
    _buggy_task->perform_async();

    WorkResult r = _buggy_task->wait();

    CPPUNIT_ASSERT( _buggy_task->ready() );
    CPPUNIT_ASSERT( r == WorkResult::Failure );
    
}

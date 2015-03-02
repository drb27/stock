#ifndef TEST_TASK_H
#define TEST_TASK_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <stocklib/task.h>

template<class Ti, class To> class problem;

class TaskTestFixture : public CppUnit::TestFixture
{
public:
    TaskTestFixture();
    virtual ~TaskTestFixture();

    void setUp();
    void tearDown();

    /** @name Test Cases */
    // @{
    void testConstruction();
    void testPerformSyncSuccess();
    void testPerformASyncSuccess();
    void testPerformAsyncThenTryAgain();
    void testReset();
    void testPerformSyncFailure();
    void testPerformAsyncFailure();
    // @}

    /** \cond internal */
    CPPUNIT_TEST_SUITE( TaskTestFixture );
    CPPUNIT_TEST( testConstruction );
    CPPUNIT_TEST( testPerformSyncSuccess );
    CPPUNIT_TEST( testPerformASyncSuccess );
    CPPUNIT_TEST( testPerformAsyncThenTryAgain );
    CPPUNIT_TEST( testReset );
    CPPUNIT_TEST( testPerformSyncFailure );
    CPPUNIT_TEST( testPerformAsyncFailure );
    CPPUNIT_TEST_SUITE_END();
    /** \endcond */

private:

    std::unique_ptr<task<long,long>> _task;
    std::unique_ptr<task<long,long>> _buggy_task;
};

#endif

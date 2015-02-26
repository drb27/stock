#ifndef TEST_STATE_H
#define TEST_STATE_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

enum class TestState { Idle, Running, Died };
enum class TestAction { Start, Pause, Stop };

template<class S, class A> class state_machine;

class StateTestFixture : public CppUnit::TestFixture
{
public:
    StateTestFixture();
    virtual ~StateTestFixture();

    void setUp();
    void tearDown();

    /** @name Test Cases */
    // @{
    void testConstructor();
    void testAddState();
    void testAddStates();
    void testAddAction();
    void testAddActions();
    void testAddAndRetrieveTransition();
    void testEntryExitActions();
    // @}

    /** \cond internal */
    CPPUNIT_TEST_SUITE( StateTestFixture );
    CPPUNIT_TEST( testConstructor );
    CPPUNIT_TEST( testAddState );
    CPPUNIT_TEST( testAddStates );
    CPPUNIT_TEST( testAddAction );
    CPPUNIT_TEST( testAddActions );
    CPPUNIT_TEST( testAddAndRetrieveTransition );
    CPPUNIT_TEST( testEntryExitActions );
    CPPUNIT_TEST_SUITE_END();
    /** \endcond */

 protected:
    state_machine<TestState,TestAction>* pMachine{nullptr};
};

#endif

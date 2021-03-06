#include <thread>
#include <exception>
#include <stdexcept>

#include "test-state.h"
#include <stocklib/state.h>

static const std::list<TestAction> actions = { TestAction::Start, TestAction::Pause, TestAction::Stop };
static const std::list<TestState> states = { TestState::Idle, TestState::Running, TestState::Died };

StateTestFixture::StateTestFixture()
{
}

StateTestFixture::~StateTestFixture()
{

}

void StateTestFixture::setUp()
{
    pMachine = new state_machine<TestState,TestAction>();

    pLoadedMachine = new state_machine<TestState,TestAction>();
    pLoadedMachine->add_states(states);
    pLoadedMachine->add_actions(actions);

    pLoadedMachine->add_transition(TestState::Idle, TestAction::Start, TestState::Running);
    pLoadedMachine->add_transition(TestState::Running, TestAction::Stop, TestState::Died);
}

void StateTestFixture::tearDown()
{
    delete pMachine;
    pMachine = nullptr;

    delete pLoadedMachine;
    pLoadedMachine = nullptr;
}

/**
 * Tests that an object can be created
 */
void StateTestFixture::testConstructor()
{
    CPPUNIT_ASSERT(pMachine!=nullptr);
}

/**
 * Tests that a state can be added
 */
void StateTestFixture::testAddState()
{
    pMachine->add_state( TestState::Idle );
    CPPUNIT_ASSERT( pMachine->has_state( TestState::Idle ) );
    CPPUNIT_ASSERT( !pMachine->has_state( TestState::Running ) );
}

/**
 * Tests that multiple states can be added
 */
void StateTestFixture::testAddStates()
{
    pMachine->add_states(states);

    for ( auto s : states )
    {
	CPPUNIT_ASSERT(pMachine->has_state(s));
    }

}

/**
 * Tests that an action can be added
 */
void StateTestFixture::testAddAction()
{
    pMachine->add_action( TestAction::Pause );
    CPPUNIT_ASSERT( pMachine->has_action( TestAction::Pause ) );
    CPPUNIT_ASSERT( !pMachine->has_action( TestAction::Start ) );
}

/**
 * Tests that multiple actions can be added
 */
void StateTestFixture::testAddActions()
{
    pMachine->add_actions(actions);

    for ( auto a : actions )
    {
	CPPUNIT_ASSERT(pMachine->has_action(a));
    }

}

/**
 * Tests that transitions can be added and retrieved
 */
void StateTestFixture::testAddAndRetrieveTransition()
{
    CPPUNIT_ASSERT( TestState::Running ==
		    pLoadedMachine->get_transition(TestState::Idle,TestAction::Start) );
}

/**
 * Tests exception on queried invalid transition
 */
void StateTestFixture::testInvalidTransition()
{
    CPPUNIT_ASSERT_THROW( pLoadedMachine->get_transition(TestState::Idle, TestAction::Stop),
			  std::logic_error);
}

/**
 * Tests exception on actioned invalid transition
 */
void StateTestFixture::testInvalidTransitionRequested()
{
    pLoadedMachine->initialize(TestState::Idle);
    CPPUNIT_ASSERT_THROW( pLoadedMachine->action(TestAction::Stop),
    			  std::logic_error);
}

/**
 * Tests that transitions can be added and retrieved
 */
void StateTestFixture::testEntryExitActions()
{
    pLoadedMachine->initialize(TestState::Idle);

    bool entryDone=false;
    bool exitDone=false;
    
    pLoadedMachine->set_entry_function(TestState::Running, [&](){entryDone=true;} );
    pLoadedMachine->set_exit_function(TestState::Running, [&](){exitDone=true;} );

    CPPUNIT_ASSERT( (!entryDone) && (!exitDone) );
    pLoadedMachine->action(TestAction::Start);
    CPPUNIT_ASSERT( (entryDone) && (!exitDone) );
    pLoadedMachine->action(TestAction::Stop);
    CPPUNIT_ASSERT( (entryDone) && (exitDone) );
    
}

/**
 * Tests blocking waiting for state entry
 */
void StateTestFixture::testWaitForStateEntry()
{
    pLoadedMachine->initialize(TestState::Idle);

    // Set up a thread which waits for transition to the running state
    bool calledFlag{false};
    
    std::thread t( [this,&calledFlag]()
		   {
		       //std::this_thread::sleep_for (std::chrono::seconds(1));
		       this->pLoadedMachine->wait_for_state_entry(TestState::Running);
		       calledFlag=true; 
		   } );
    
    pLoadedMachine->action(TestAction::Start);
    t.join();

    CPPUNIT_ASSERT(calledFlag==true);
}

/**
 * Tests entry functions
 */
void StateTestFixture::testEntryFunction()
{
    pLoadedMachine->initialize(TestState::Idle);

    bool functionCalled=false;

    pLoadedMachine->set_entry_function(TestState::Running,
				       [&]()
				       {
					   functionCalled=true;
				       } );

    CPPUNIT_ASSERT(!functionCalled);
    pLoadedMachine->action(TestAction::Start);
    CPPUNIT_ASSERT(functionCalled);
}

/**
 * Tests exit functions
 */
void StateTestFixture::testExitFunction()
{
    pLoadedMachine->initialize(TestState::Idle);

    bool functionCalled=false;

    pLoadedMachine->set_exit_function(TestState::Idle,
				      [&]()
				      {
					  functionCalled=true;
				      } );

    CPPUNIT_ASSERT(!functionCalled);
    pLoadedMachine->action(TestAction::Start);
    CPPUNIT_ASSERT(functionCalled);

}

/**
 * Tests the initialization routine
 */
void StateTestFixture::testInitialize()
{
    bool functionCalled=false;

    pLoadedMachine->set_entry_function(TestState::Idle,
				       [&]()
				       {
					   functionCalled=true;
				       } );

    pLoadedMachine->initialize(TestState::Idle);

    CPPUNIT_ASSERT(pLoadedMachine->get_state()==TestState::Idle);
    CPPUNIT_ASSERT(functionCalled);
}

/**
 * Tests that holding an explicit lock does not cause deadlocks.
 */
void StateTestFixture::testHoldExplicitLock()
{
    pLoadedMachine->initialize(TestState::Idle);
    std::unique_lock<std::recursive_mutex> lock = pLoadedMachine->obtain_lock();

    // Start a new thread, which attempts to issue the Stop action
    std::thread t( [this]()
		   {
		       this->pLoadedMachine->action(TestAction::Stop);
		   } );

    pLoadedMachine->action( TestAction::Start );
    
    CPPUNIT_ASSERT( pLoadedMachine->get_state()==TestState::Running );
    
    // Sleep a little while, then check again
    std::this_thread::sleep_for( std::chrono::milliseconds(100) );

    CPPUNIT_ASSERT( pLoadedMachine->get_state()==TestState::Running );

    // Release the explicit lock, the second thread should now have access, and 
    // complete its transition
    lock.unlock();

    pLoadedMachine->wait_for_state_entry( TestState::Died );
    t.join();

    CPPUNIT_ASSERT( pLoadedMachine->get_state()==TestState::Died );
}

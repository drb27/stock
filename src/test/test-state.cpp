#include "test-state.h"
#include "../stock/state.h"

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
}

void StateTestFixture::tearDown()
{
    delete pMachine;
    pMachine = nullptr;
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
    pMachine->add_states(states);
    pMachine->add_actions(actions);

    pMachine->add_transition(TestState::Idle, TestAction::Start, TestState::Running);
    CPPUNIT_ASSERT( TestState::Running == pMachine->get_transition(TestState::Idle,TestAction::Start) );
}

/**
 * Tests that transitions can be added and retrieved
 */
void StateTestFixture::testEntryExitActions()
{
    pMachine->add_states(states);
    pMachine->add_actions(actions);

    pMachine->add_transition(TestState::Idle, TestAction::Start, TestState::Running);

    pMachine->initialize(TestState::Idle);

    bool entryDone=false;
    bool exitDone=false;
    
    pMachine->set_entry_function(TestState::Running, [&](){entryDone=true;} );
    pMachine->set_exit_function(TestState::Idle, [&](){exitDone=true;} );

    CPPUNIT_ASSERT( (!entryDone) && (!exitDone) );
    pMachine->action(TestAction::Stop);
    CPPUNIT_ASSERT( (!entryDone) && (!exitDone) );
    pMachine->action(TestAction::Start);
    CPPUNIT_ASSERT( (entryDone) && (exitDone) );
    
}

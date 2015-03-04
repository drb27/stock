/**
 * @file
 */

/*
The MIT License (MIT)

Copyright (c) 2015 David Bradshaw

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef STATE_H
#define STATE_H

#include <type_traits>
#include <condition_variable>
#include <mutex>
#include <set>
#include <map>
#include <list>
#include <exception>
#include <stdexcept>
#include <functional>

#define LOCK std::lock_guard<std::recursive_mutex>(this->_mutex)
#define LOCK2 std::lock_guard<std::mutex>(this->_statechange_mutex)

/**
 * A thread-safe state machine implementation, with a dynamic definition which
 * can be modified at run-time.
 *
 * Possible modifications include the ability to change state transitions, state
 * entry callback actions, and state exit callback actions. 
 *
 * Most public member functions are protected by a mutex, preventing
 * simultaneous access from multiple threads. However, the mutex is recursive,
 * meaning that these members are re-entrant, and it is possible to call them
 * from each other (e.g. during a callback). This is handy in preventing
 * deadlocks, while maintaining thread safety. 
 *
 * States are self-explanatory. Actions are the stimuli that cause changes in
 * state to occur. Transitions are changes from one state to another in response
 * to an action. 
 *
 * @param S A strongly-typed enumeration describing the states
 * @param A A strongly-typed enumeration describing the actions
 */
template<class S,class A>
class state_machine
{
    static_assert(std::is_enum<S>(),"Template parameters for state_machine must be an enumeration");
    static_assert(std::is_enum<A>(),"Template parameters for state_machine must be an enumeration");

public:

    typedef std::map<A,S> action_table;
    typedef std::map<S,action_table> transition_table;
    typedef std::map<S,std::function<void()>> function_table;

    virtual ~state_machine()
    {
    }


    /**
     * Adds a state to the state machine. You need to manually create each state.
     */
    void add_state(S s)  { LOCK; add_state_bare(s); }

    /**
     * Adds an action to the state machine. You need to manually create each action.
     */
    void add_action(A a) { LOCK; add_action_bare(a); }

    /**
     * Adds multiple states to the state machine. Equivalent to calling
     * add_state() on each member of the list. 
     */
    void add_states( const std::list<S>& s)  { LOCK; for ( auto st : s ) add_state_bare(st); }

    /**
     * Adds multipe actions to the state machine. Equivalent to calling
     * add_action() on each member of the list. 
     */
    void add_actions( const std::list<A>& a) { LOCK; for ( auto ac : a ) add_action_bare(ac); }

    /**
     * Determines if a state has been added with add_state() or add_states() 
     */
    bool has_state(S s) const { LOCK; return has_state_bare(s); }

    /**
     * Determines if an action has been added with add_action() or add_actions()
     */
    bool has_action(A a) const { LOCK; return has_action_bare(a); }

    /**
     * Adds a transition to the state machine definition. You should only add
     * one transition for each (os,ac) pair. Behaviour is undefined if you
     * violate this. 
     *
     * @param os the starting state
     * @param ac the action causing the transition
     * @param ns the new state
     */
    void add_transition(S os, A ac, S ns) { LOCK; add_transition_bare(os,ac,ns); }

    /**
     * Returns the state that would result if the given action is received from
     * the given state
     * 
     * @param os The state
     * @param ac The action
     * @return The hypothetical new state. A transition does not actually
     * occur. 
     */
    S get_transition(S os, A ac) const { LOCK; return get_transition_bare(os,ac); } 

    /**
     * Initializes the state machine to the given initial state. The state must
     * have previously been added with add_state() or add_states(). This method
     * should be called before any transitions are attempted. 
     *
     * @param initial The state the machine assumes as its initial state. 
     *
     * @note If defined, the entry actions for the state will be executed
     * before this call returns. 
     */
    void initialize(S initial) const { LOCK; _state=initial; do_entry_actions_bare(); }

    /**
     * Queries the state the state machine is currently in.
     * @return The identifier of the current state
     */
    S get_state() const { LOCK; return _state; }

    /**
     * Performs on action on the state machine. Note that this may have no
     * effect if an applicable transition has not been defined.
     * 
     * @param a The action to perform
     */
    void action(A a) const { LOCK; LOCK2; action_bare(a); } 

    /**
     * Defines entry actions for the given state. Whenever a transition to the
     * given state occurs, the given function is executed immediately following
     * the state change, in the caller's thread, before this API call returns.
     *
     * @param s The state to which the entry function is applied
     * @param f The function to call each time the state is entered 
     */
    void set_entry_function(S s, std::function<void()> f) { LOCK; set_entry_function_bare(s,f); }

    /**
     * Defines exit actions for the given state. Whenever a transition out of
     * the given state occurs, the given function is executed immediately prior
     * to the state change, in the caller's thread, before this API call
     * returns.
     *
     * @param s The state to which the exit function is applied
     * @param f The function to call each time the state is exited
     *
     * @note The exit function will not be called upon destruction of the
     * object.  
     */
    void set_exit_function(S s, std::function<void()> f)  { LOCK; set_exit_function_bare(s,f); }

    /**
     * Throws a std::logic_error exception if the state machine is not in the
     * given state. If you need to ensure that no state change occurs following
     * this call, obtain a lock first with obtain_lock(), then call this. No
     * state change can occur until the lock is released. It is still safe to
     * call other methods of the object from the same thread - only other
     * threads will be blocked.
     *
     * @param s The state to test for
     */
    void ensure_state(S s) const { LOCK; if (s!=_state) throw std::logic_error(); }

    /**
     * Obtains a lock on the object. No state changes will be possible from
     * other threads until the lock is released by either going out of scope or
     * by calling unlock() upon it. The calling thread that obtains the lock can
     * still operate freely on the state machine, including initiating a
     * transition.
     *
     * @return a lock which prevents access to the state machine from other threads.
     */
    std::unique_lock<std::recursive_mutex> obtain_lock() const
    {
	return std::unique_lock<std::recursive_mutex>(_mutex);
    }

    /**
     * Blocks the calling thread indefinitely until the given state is
     * entered. If the state machine is already in the given state, this
     * function returns immediately.
     *
     * @param s The state to wait for
     */
    void wait_for_state_entry(S s) const
    {
	std::unique_lock<std::recursive_mutex> main_lock(_mutex);
	std::unique_lock<std::mutex> event_lock(_statechange_mutex);
	event_lock.unlock();

	if (_state==s)
	{
	    // We're already done. Release the main mutex and return.
	    main_lock.unlock();
	    return;
	}
	
	// Now lock the statechange lock and release the main lock. Another thread
	// must own both in order to change the state.
	event_lock.lock();
	main_lock.unlock();

	bool achievedState=false;
	while(!achievedState)
	{
	    _state_change.wait(event_lock);
	    if (_state==s)
		achievedState=true;
	}
    }

protected:

    bool is_valid_transition_bare(A a) const
    {
	const auto& table = _tt.at(_state);
	const auto i = table.find(a);
	return (i!=table.end());
    }

    void do_entry_actions_bare() const
    {
	if (has_entry_function_bare(_state))
	    _entry_actions.at(_state)();
    }

    void do_exit_actions_bare() const
    {
	if (has_exit_function_bare(_state))
	    _exit_actions.at(_state)();
    }

    void action_bare(A a) const 
    {
	if (is_valid_transition_bare(a))
	{
	    S newState = get_transition_bare(_state,a);

	    // Exit actions
	    do_exit_actions_bare();

	    // Enter the new state
	    _state = newState;

	    // Perform new state entry actions
	    do_entry_actions_bare();
	    
	    // Notify waiters
	    _state_change.notify_all();
	    
	}
	else
	    throw std::logic_error("Invalid action for current state");
    }

    void add_state_bare(S s)  
    { 
	if (!has_state_bare(s))
	{
	    _states.insert(s);
	    _tt[s] = (action_table){};
	}
    }

    void add_transition_bare(S os, A ac, S ns)
    {
	auto& table = _tt[os];
	table[ac] = ns;
    }

    S get_transition_bare(S os, A ac) const
    {
	const auto& table = _tt.at(os);
	const auto i = table.find(ac);
	if (i!=table.end())
	    return (*i).second;
	else
	    throw std::logic_error("Undefined transition");
    }
    
    void add_action_bare(A a) { _actions.insert(a); }

    bool has_state_bare(S s) const { return _states.find(s)!=_states.end(); }
    bool has_action_bare(A a) const { return _actions.find(a)!=_actions.end(); }

    void set_entry_function_bare(S s, std::function<void()>& f)
    {
	_entry_actions[s] = f;
    }

    void set_exit_function_bare(S s, std::function<void()>& f)
    {
	_exit_actions[s] = f;
    }

    bool has_entry_function_bare(S s) const
    {
	return _entry_actions.find(s)!=_entry_actions.end();
    }

    bool has_exit_function_bare(S s) const
    {
	return _exit_actions.find(s)!=_exit_actions.end();
    }


private:
    mutable std::recursive_mutex _mutex;
    mutable std::mutex _statechange_mutex;
    std::set<S> _states;
    std::set<A> _actions;
    transition_table _tt;
    mutable S _state;
    function_table _entry_actions;
    function_table _exit_actions;
    mutable std::condition_variable _state_change;

};

#endif

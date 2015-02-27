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

#define LOCK std::lock_guard<std::mutex>(this->_mutex)
#define LOCK2 std::lock_guard<std::mutex>(this->_statechange_mutex)

/**
 * A thread-safe state machine implementation, with a dynamic definition which
 * can be modified at run-time. (Actions only, not states. States are compile-time.
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


    void add_state(S s)  { LOCK; add_state_bare(s); }
    void add_action(A a) { LOCK; add_action_bare(a); }

    void add_states( const std::list<S>& s)  { LOCK; for ( auto st : s ) add_state_bare(st); }
    void add_actions( const std::list<A>& a) { LOCK; for ( auto ac : a ) add_action_bare(ac); }

    bool has_state(S s) const { LOCK; return has_state_bare(s); }
    bool has_action(A a) const { LOCK; return has_action_bare(a); }

    void add_transition(S os, A ac, S ns) { LOCK; add_transition_bare(os,ac,ns); }

    S get_transition(S os, A ac) const { LOCK; return get_transition_bare(os,ac); } 

    void initialize(S initial) const { LOCK; _state=initial; do_entry_actions_bare(); }

    S get_state() const { LOCK; return _state; }

    void action(A a) const { LOCK; LOCK2; action_bare(a); } 

    void set_entry_function(S s, std::function<void()> f) { LOCK; set_entry_function_bare(s,f); }
    void set_exit_function(S s, std::function<void()> f)  { LOCK; set_exit_function_bare(s,f); }

    void wait_for_state_entry(S s) const
    {
	std::unique_lock<std::mutex> main_lock(_mutex);
	std::unique_lock<std::mutex> event_lock(_statechange_mutex);
	event_lock.release();

	if (_state==s)
	{
	    // We're already done. Release the main mutex and return.
	    main_lock.release();
	    return;
	}
	
	// Now lock the statechange lock and release the main lock. Another thread
	// must own both in order to change the state.
	event_lock.lock();
	main_lock.release();

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
	    
	    do_exit_actions_bare();
	    _state = newState;
	    do_entry_actions_bare();

	    _state_change.notify_all();
	}
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
    mutable std::mutex _mutex;
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

#ifndef PROBLEM_H
#define PROBLEM_H

#include <functional>

/**
 * Template class representing a computational problem to be solved. 
 * 
 * Upon construction, objects of this class store away a method and a parameter. 
 * When asked to solve the problem, the object executes the method, passing in the
 * parameter, and returns the output of the method. 
 *
 * This class is templatized on the parameter type and the return value type. The 
 * method type is implicitly Ti->To. 
 * 
 * @param Ti The parameter type
 * @param To The return value type
 */
template<class Ti,class To>
class problem
{
public:
    
    /**
     * Represents the type of the method Ti->To
     */
    typedef To fn_t(Ti);

protected:

    const std::function<fn_t> _f;
    const Ti _p;

private:

public:

    /**
     * Constructor. Takes a method, and a parameter.
     */
    problem( const std::function<fn_t> f, const Ti& p) : _f(f), _p(p)
    {
    }

    /**
     * Move constructor.
     */
    problem( problem&& p ) : _f(std::move(p._f)), _p(std::move(p._p)) 
    {
    }

    /**
     * Copy constructor
     */
    problem( const problem& o ) : _f(o._f), _p(o._p)
    {
    }

    /**
     * Move assignment operator is denied (const members)
     */
    problem<Ti,To>& operator=( problem<Ti,To>&& other )=delete;

    /**
     * Copy assignment operator is denied (const members)
     */
    problem<Ti,To>& operator=( const problem<Ti,To>& other)=delete;

    /**
     * Solves the problem. Applies the parameter to the method, and returns
     * the return value of the method. 
     */
    To solve() const
    {
	return _f(_p);
    }

    /**
     * Functor (shorthand for solve())
     */
    To operator()(void) const
    {
	return solve();
    }

};

#endif

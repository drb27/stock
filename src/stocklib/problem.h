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

#ifndef PROBLEM_H
#define PROBLEM_H

#include <functional>
#include <exception>
#include <stdexcept>

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

    /**
     * Class representing a general problem executing the problem. 
     * Thrown by solve() and operator() if any other exception is
     * thrown by the problem method. 
     */
    class abort_exception : public std::logic_error
    {
    public:
	
	explicit abort_exception() : std::logic_error("An unknown error occurred while solving a problem") 
	{
	}

	explicit abort_exception(const std::string& s) : std::logic_error(s)
	{
	} 
	
	explicit abort_exception(const std::exception& e) 
	    : std::logic_error("An error occurred while solving a problem"),
	      _inner(e)
	{
	}

    protected:
	std::exception _inner;
    };

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

    virtual ~problem() {}

    /**
     * Move constructor.
     */
    problem( problem&& p )=delete;

    /**
     * Copy constructor
     */
    problem( const problem& o )=delete;

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
	try
	{
	    return _f(_p);
	}
	catch( const std::exception& e )
	{
	    throw abort_exception(e);
	}
    }

    /**
     * Functor (shorthand for solve())
     */
    To operator()(void) const
    {
	return solve();
    }

};

/**
 * A problem which encapsulates its algorithm. 
 *
 * Unlike a regular problem object, contained_problem objects do not take a
 * functor paramter during construction. Instead, the do_work() method is
 * implemented, which gets called during the evaluation of the problem. The
 * advantage of doing so is that the algorithm is executed in object context,
 * meaning it has access to the classes methods and data. 
 *
 * @param Ti The type of the input to the problem
 * @param To The type of the output from the problem
 *
 */
template<class Ti,class To>
class contained_problem : public problem<Ti,To>
{
public:

    /**
     * Constructs the problem, along with the input data. 
     * @param p The input data to be passed to do_work() during problem evaluation.
     */
    contained_problem(const Ti& p) : problem<Ti,To>( [this](Ti i)->To
	{
	    return this->do_work(i);
	}
	,p) 
    {
    }

    contained_problem( contained_problem&& o)=delete;
    contained_problem( const contained_problem& o)=delete;
    contained_problem& operator=( const contained_problem& )=delete;
    contained_problem& operator=( const contained_problem&& )=delete;

    virtual ~contained_problem()
    {
    }

protected:

    /**
     * Encapsulates the algorithm of the problem. Implement this in child
     * classes to solve the problem. Return the output. 
     *
     * @param i The input data to the problem
     * @return The result of the problem. 
     *
     * @note Note that in addition to the input parameter i, the method has
     * access to all of the objects member data and member functions.
     */
    virtual To do_work(Ti i)=0;
};

#endif

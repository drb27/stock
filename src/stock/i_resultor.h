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

#ifndef I_RESULTOR_H
#define I_RESULTOR_H

#include <exception>
#include <type_traits>

/**
 * Interface definition for a class which can return a result.
 * The result type is parameterized.
 * 
 * @param R The type of the result to be returned.
 * @param E The exception type to use
 *
 * @note The default value of the result immediately after construction is zero,
 *       regardless of whether or not this is a valid  
 */
template<class R,class E>
class i_resultor
{
    static_assert(std::is_enum<R>::value,"i_resultor can only be used with enumerations");

public:

    /**
     * Default constructor. Allows child classes to specify a default value for the
     * result.
     */
    i_resultor( const R& initial = R() ) : _result(initial) {} 

    /**
     * Returns a reference to the current result, if available. 
     * If not available, std::logic_error is thrown.
     */
    const R& result() const 
    {
	if (_ready)
	    return _result;
	else
	    throw std::logic_error("The requested result is not available.");
    }

    /**
     * Gets the current exception
     * @return a const reference to the current exception object. 
     */
    const E& exception() const
    {
	return _exception;
    }

    /**
     * Determines if a result is available. 
     * @return true if a result is available, false otherwise
     */
    bool ready() const
    {
	return _ready;
    }
    
protected:

    /**
     * Called by child classes to set or clear the ready flag
     * @param r Call with true to indicate a result is ready, false otherwise
     */
    void set_ready(bool r=true ) 
    {
	_ready=r;
    }

    /** 
     * Called by child classes to set the result. 
     * The ready flag is automatically set.
     * @param r The result to store.
     */
    void set_result( const R& r )
    {
	_result = r;
	set_ready();
    }

    /**
     * Called by child classes to set the result code, with an exception.
     * Typically used when the result code indicates some kind of error. 
     * The ready flag is automatically set.
     * @param r The result to store (a copy is made)
     * @param e The exception to store (a copy is made)
     */
    void set_result( const R& r, const E& e )
    {
	_exception = e;
	set_result(r);
    }

    void reset_result(const R& initial)
    {
	_exception = E();
	_result = initial;
	_ready = false;
    }

private:
    bool _ready{false};
    R _result{R()};
    E _exception{E()};
    
};

#endif

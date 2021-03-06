/**
 * @file 
 * This is a class that adds objects of type T to a list, and then when it goes out of scope, 
 * performs a lambda function, passing each one as a parameter. 
 *
 * T = Type of the object added to the list
 *
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

#ifndef SWEEPUP_H
#define SWEEPUP_H

#include <list>
#include <functional>

/** @class sweepup
 * A class which holds a list of objects, and then executes a function
 * upon each when it is destructed. 
 *
 * An object of type sweepup holds a temporary list of objects, the intention being
 * to perform a consistent operation on each object at some later time. For example, a list
 * of resources which is later to be deleted. Objects of this class take a functor in the
 * constructor, and then objects are added via add(). When the sweepup object goes out of
 * scope, the functor is executed once per object added via add(), with the object as an
 * argument to the function.
 *
 * @note A common way to use this class is to create an instance on the stack. When the
 * object goes out of scope, the sweepup action is performed on each listed resource. */
template<class T>
class sweepup
{

 public:
    
    /** 
     * The type of the action function 
     * 
     * The type of the function passed to the constructor that will
     * be performed on each element upon destruction of this object.
     * @note Derived from T, i.e. (lambda) T& --> void
     */
    typedef void (action_func)(T&);

    /** @name Lifecycle Management */
    //@{

    /**
     * Constructor taking a function argument. 
     *
     * @param act A function, functor or lambda defining the action to be taken with
     * each element.
     */
    sweepup(std::function<action_func> act) : action(act) {}

    /**
     * Invokes the clean-up action
     *
     * At the point of destruction, this object will invoke the action function, passed
     * as an argument to the constructor, once per object added via add(), passing the
     * object to the action function 
     */
    virtual ~sweepup()
	{
	    for ( T* el : _list ) 
		action(*el);
	}
    //@}

    /** @name Public API */
    ///@{

    /**
     * Adds an element to the list of elements to be processed.
     *
     * When this object goes out of scope, a pointer to this object will be passed to
     * the function given to the constructor.
     *
     * @param element Takes a reference to an element of type T
     * @return Nothing
     */
    virtual void add(T& element)
    {
	_list.push_back(&element);
    }

    ///@}

 protected:
    std::list<T*> _list;	        ///< Stores the list of objects
    std::function<action_func> action;	///< Stores the action to perform 
};

#endif

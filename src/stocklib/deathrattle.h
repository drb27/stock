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

#ifndef DEATHRATTLE_H
#define DEATHRATTLE_H

#include <functional>

/**
 * Performs an action when a deathrattle instance goes out of scope
 *
 * A deathrattle instance triggers an action when it goes out of scope. The action is
 * represented by a function object, which is passed to the constructor.
 *
 * Example:
 * @code
 * {
 *     SomeResource* resource = new SomeResource();
 *     deathrattle d( [](){ cleanup(resource); } );
 *     
 *     // Do some stuff ...
 * }
 * @endcode

 * @note Note that you can pass a traditional function pointer, a std::function object, or
 * a lambda expression to the constructor.
 *
 * @warning Be cautious of relying on a deathrattle with global scope! 
 */
class deathrattle
{
public:

    typedef void (voidfunc)(void);

    /*!
     * Constructor 
     * @param f a function pointer, a function object, or a lambda expression
     */
    deathrattle( const std::function<voidfunc> f) : _f(f) {}

    /**
     * Aborts the deathrattle. The action will not be triggered when the object goes out of scope.
     */
    void abort() { aborted=true; }

    /**
     * Destructor. Triggers the action passed to the constructor. 
     */
    virtual ~deathrattle();

protected:

    const std::function<voidfunc> _f; /**<  Holds the action */
    bool aborted=false;

};

#endif

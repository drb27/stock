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

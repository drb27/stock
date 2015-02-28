#ifndef I_RESULTOR_H
#define I_RESULTOR_H

#include <exception>
#include <type_traits>

/**
 * Interface definition for a class which can return a result.
 * The result type is parameterized.
 * 
 * @param R The type of the result to be returned.
 *
 * @note The default value of the result immediately after construction is zero,
 *       regardless of whether or not this is a valid  
 */
template<class R>
class i_resultor
{
    static_assert(std::is_enum<R>::value,"i_resultor can only be used with enumerations");

public:
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
    const std::exception& exception() const
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
    
    /**
     * Returns a reference to the current exception.
     * Typically only useful if the result code indicates a failure
     */
    const std::exception& error() const { return _exception; }

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
    void set_result( const R& r, const std::exception& e )
    {
	_exception = e;
	set_result(r);
    }

private:
    bool _ready{false};
    R _result ={R()};
    std::exception _exception = std::exception();
    
};

#endif

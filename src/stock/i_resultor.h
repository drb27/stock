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
     * Returns a reference to the current result.
     */
    const R& result() const { return _result; }
    
    /**
     * Returns a reference to the current exception.
     * Typically only useful if the result code indicates a failure
     */
    const std::exception& error() const { return _exception; }

protected:
    R _result = R();
    std::exception _exception = std::exception();
};

#endif

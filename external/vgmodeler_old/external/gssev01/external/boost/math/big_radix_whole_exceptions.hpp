//  Boost math/big_radix_whole_exceptions.hpp header file  -------------------//

//  Copyright 2006 Daryle Walker.  Distributed under the Boost Software License,
//  Version 1.0.  (See the accompanying file LICENSE_1_0.txt or a copy at
//  <http://www.boost.org/LICENSE_1_0.txt>.)

//  See <http://www.boost.org/libs/math/> for the library's home page.

/** \file
    \brief  Exception types from arbitrary-length specific-radix unsigned
            integers

    Contains the definitions of various types that can be thrown from operations
    involving objects of the \c boost::math::big_radix_whole\<\> class template.
    (These are types explicitly thrown from the operations' algorithms, and not
    any system, language, or other exceptions thrown implicitly by an operation
    of an internal component.)
 */

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_EXCEPTIONS_HPP
#define BOOST_MATH_BIG_RADIX_WHOLE_EXCEPTIONS_HPP

#include <stdexcept>  // for std::domain_error, std::underflow_error, etc.
#include <string>     // for std::string


namespace boost
{
namespace math
{


//  Forward declarations  ----------------------------------------------------//

// Error-indication types
struct big_radix_whole_error;
class  big_radix_whole_divide_by_zero_error;
class  big_radix_whole_negative_result_error;
class  big_radix_whole_bad_format_error;
class  big_radix_whole_conversion_error;


//  Radix-specific bignum unsigned integer exception class declarations  -----//

/** \brief  Represents general errors from arbitrary-length specific-radix
            unsigned integers

    A base class to mark all exceptions that come from an operation involving
    \c big_radix_whole\<\> objects, not including allocations.  Catch a
    reference to this type to see if a \c big_radix_whole\<\> object failed.
    You need to catch or dynamic-cast to a derived type to go further.
 */
struct big_radix_whole_error
{
    // No public members

protected:
    // Lifetime management
    //! Destructor
    virtual  ~big_radix_whole_error() throw();

};  // boost::math::big_radix_whole_error

/** \brief  Represents division errors from arbitrary-length specific-radix
            unsigned integers

    An exception class thrown whenever a \c big_radix_whole\<\> object causes an
    error by using a zero divisor during a division and/or modulus operation.
 */
class big_radix_whole_divide_by_zero_error
    : public virtual big_radix_whole_error
    , public         std::domain_error
{
public:
    // Lifetime management
    //! Construct with an error message
    explicit  big_radix_whole_divide_by_zero_error( std::string const &what );

};  // boost::math::big_radix_whole_divide_by_zero_error

/** \brief  Represents subtraction errors from arbitrary-length specific-radix
            unsigned integers

    An exception class thrown whenever a \c big_radix_whole\<\> object causes an
    error by getting a negative result during a subtraction, decrement, or
    normalizing operation.
 */
class big_radix_whole_negative_result_error
    : public virtual big_radix_whole_error
    , public         std::underflow_error
{
public:
    // Lifetime management
    //! Construct with an error message
    explicit  big_radix_whole_negative_result_error( std::string const &what
     );

};  // boost::math::big_radix_whole_negative_result_error

/** \brief  Represents format-conversion errors from arbitrary-length
            specific-radix unsigned integers

    An exception class thrown whenever a \c big_radix_whole\<\> object is
    initialized by a string that does not have the correct format.
 */
class big_radix_whole_bad_format_error
    : public virtual big_radix_whole_error
    , public         std::invalid_argument
{
public:
    // Lifetime management
    //! Construct with an error message
    explicit  big_radix_whole_bad_format_error( std::string const &what );

};  // boost::math::big_radix_whole_bad_format_error

/** \brief  Represents conversion-overflow errors from arbitrary-length
            specific-radix unsigned integers

    An exception class thrown whenever a \c big_radix_whole\<\> object has a
    value too large for a receiving numeric type of conversion.
 */
class big_radix_whole_conversion_error
    : public virtual big_radix_whole_error
    , public         std::overflow_error
{
public:
    // Lifetime management
    //! Construct with an error message
    explicit  big_radix_whole_conversion_error( std::string const &what );

};  // boost::math::big_radix_whole_conversion_error


}  // namespace math
}  // namespace boost


//  Radix/bignum/natural exception constructor/destructor definitions  -------//

/** Does nothing.  It only exists to be virtual so a caught reference to a
    \c big_radix_whole_error object can be \c dynamic_cast to the
    appropriate derived class.
 */
inline
boost::math::big_radix_whole_error::~big_radix_whole_error
(
) throw()
{
}

/** Does nothing but pass on the error message to a base class.

    \param what  The error message to be extracted later with the inherited
                 \c what member function

    \post  <code>0 == std::strcmp( this-&gt;what(), <var>what</var>.c_str()
           )</code>
 */
inline
boost::math::big_radix_whole_divide_by_zero_error::
 big_radix_whole_divide_by_zero_error
(
    std::string const &  what
)
    : std::domain_error( what )
{
}

/** Does nothing but pass on the error message to a base class.

    \param what  The error message to be extracted later with the inherited
                 \c what member function

    \post  <code>0 == std::strcmp( this-&gt;what(), <var>what</var>.c_str()
           )</code>
 */
inline
boost::math::big_radix_whole_negative_result_error::
 big_radix_whole_negative_result_error
(
    std::string const &  what
)
    : std::underflow_error( what )
{
}

/** Does nothing but pass on the error message to a base class.

    \param what  The error message to be extracted later with the inherited
                 \c what member function

    \post  <code>0 == std::strcmp( this-&gt;what(), <var>what</var>.c_str()
           )</code>
 */
inline
boost::math::big_radix_whole_bad_format_error::big_radix_whole_bad_format_error
(
    std::string const &  what
)
    : std::invalid_argument( what )
{
}

/** Does nothing but pass on the error message to a base class.

    \param what  The error message to be extracted later with the inherited
                 \c what member function

    \post  <code>0 == std::strcmp( this-&gt;what(), <var>what</var>.c_str()
           )</code>
 */
inline
boost::math::big_radix_whole_conversion_error::big_radix_whole_conversion_error
(
    std::string const &  what
)
    : std::overflow_error( what )
{
}


#endif  // BOOST_MATH_BIG_RADIX_WHOLE_EXCEPTIONS_HPP

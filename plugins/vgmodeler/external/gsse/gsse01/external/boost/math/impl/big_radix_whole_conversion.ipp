//  Boost math/impl/big_radix_whole_conversion.ipp header file  --------------//

//  Copyright 2006 Daryle Walker.  Distributed under the Boost Software License,
//  Version 1.0.  (See the accompanying file LICENSE_1_0.txt or a copy at
//  <http://www.boost.org/LICENSE_1_0.txt>.)

//  See <http://www.boost.org/libs/math/> for the library's home page.

/** \file
    \brief  Part of the implementation for an arbitrary-length specific-radix
            unsigned integer class template

    Contains the definitions some of the public member functions of the
    \c boost::math::big_radix_whole\<\> class template.  These member functions
    present an object's value as another type.
 */

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_IMPL_CONVERSION_IPP
#define BOOST_MATH_BIG_RADIX_WHOLE_IMPL_CONVERSION_IPP

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_CORE_HPP
#error "#Include only as part of <boost/math/big_radix_whole_core.hpp>"
#endif

#include <algorithm>  // for std::reverse
#include <cstdlib>    // for std::div, div_t
#include <limits>     // for std::numeric_limits
#include <string>     // for std::string

#include <boost/cstdint.hpp>        // for boost::uintmax_t
#include <boost/static_assert.hpp>  // for BOOST_STATIC_ASSERT


//! \cond
//  Implementation details  --------------------------------------------------//

namespace boost
{
namespace math
{
namespace detail
{

template < typename DigitReverseIterator, typename NumericType, typename
 ExceptionType >
NumericType
convert_digit_string_to_bounded_number
(
    DigitReverseIterator const  digit_string_rbegin,
    DigitReverseIterator const  digit_string_rend,
    NumericType const           radix,
    ExceptionType const &       overflow_exception
)
{
    typedef ::std::numeric_limits<NumericType>  limits_type;

    BOOST_STATIC_ASSERT(limits_type::is_specialized && limits_type::is_bounded);

    NumericType        result( 0 );
    NumericType const  hard_limit = limits_type::max();

    for ( DigitReverseIterator  i = digit_string_rbegin ; digit_string_rend != i
     ; ++i )
    {
        NumericType const  next_digit = *i;

        if ( result > (hard_limit - next_digit) / radix )
        {
            throw overflow_exception;
        }
        else
        {
            result *= radix;
            result += next_digit;
        }
    }

    return result;
}

template < typename DigitReverseIterator, typename NumericType >
NumericType
convert_digit_string_to_unbounded_number
(
    DigitReverseIterator const  digit_string_rbegin,
    DigitReverseIterator const  digit_string_rend,
    NumericType const           radix
)
{
    NumericType  result( 0 );

    for ( DigitReverseIterator  i = digit_string_rbegin ; digit_string_rend != i
     ; ++i )
    {
        result *= radix;
        result += *i;
    }

    return result;
}

template < bool IsBounded >
struct convert_digit_string_to_number_action
{
    template < typename Iterator, typename Number, typename Throw >
    Number  operator ()( Iterator dsb, Iterator dse, Number r, Throw const &oe )
    { return convert_digit_string_to_bounded_number( dsb, dse, r, oe ); }
};

template < >
struct convert_digit_string_to_number_action<false>
{
    template < typename Iterator, typename Number, typename Throw >
    Number  operator ()( Iterator dsb, Iterator dse, Number r, Throw const & )
    { return convert_digit_string_to_unbounded_number( dsb, dse, r ); }
};

template < typename DigitReverseIterator, typename NumericType, typename
 ExceptionType >
NumericType
convert_digit_string_to_number
(
    DigitReverseIterator   digit_string_rbegin,
    DigitReverseIterator   digit_string_rend,
    NumericType            radix,
    ExceptionType const &  overflow_exception
)
{
    typedef ::std::numeric_limits<NumericType>  limits_type;

    return convert_digit_string_to_number_action<limits_type::is_specialized &&
     limits_type::is_bounded>()( digit_string_rbegin, digit_string_rend, radix,
     overflow_exception );
}

template < bool IsSuperDecimal >
struct get_digit_separator_string_action
{
    char const *  operator ()()
    { static  char const  result[] = "-";  return result; }
};

template < >
struct get_digit_separator_string_action<false>
{
    char const *  operator ()()
    { static  char const  result[] = "";  return result; }
};

template < int Radix >
char const *
get_digit_separator_string
(
)
{
    return get_digit_separator_string_action<(Radix < -10) || (Radix > 10)>()();
}

void
write_integer_digits
(
    uintmax_t  in_value,
    char *     out_string
)
{
    char * const  start = out_string;

    do
    {
        std::div_t const  qr = std::div( in_value, 10 );

        *out_string++ = qr.rem + '0';
        in_value = qr.quot;
    }
    while ( in_value );

    std::reverse( start, out_string );
    *out_string = '\0';
}

}  // namespace detail
}  // namespace math
}  // namespace boost
//! \endcond


//  Radix/bignum/natural extra member function definitions  ------------------//

/** Returns the integral value of the current object.

    \return  The object's equivalent value as a built-in integer

    \throws boost::math::big_radix_whole_conversion_error
             The current value is larger than what a built-in integer can store.
 */
template < int Radix, class Allocator >
inline
boost::uintmax_t
boost::math::big_radix_whole<Radix, Allocator>::to_uintmax
(
) const
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    return detail::convert_digit_string_to_number( this->digits_.rbegin(),
     this->digits_.rend(), static_cast<uintmax_t>(self_type::radix),
     big_radix_whole_conversion_error("value too large for an uintmax_t") );
}

/** Returns the floating value of the current object.

    \return  The object's equivalent value as a built-in float

    \throws boost::math::big_radix_whole_conversion_error
             The current value is larger than what a \c double can store.
 */
template < int Radix, class Allocator >
inline
double
boost::math::big_radix_whole<Radix, Allocator>::to_double
(
) const
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    return detail::convert_digit_string_to_number( this->digits_.rbegin(),
     this->digits_.rend(), static_cast<double>(self_type::radix),
     big_radix_whole_conversion_error("value too large for a double") );
}

/** Returns a string containing a digit expansion of the current object's value.
    For radices larger than 10, hyphens separate each digit (which are each
    represented in decimal).

    \return  A string representing the object's numeric value.
 */
template < int Radix, class Allocator >
std::string
boost::math::big_radix_whole<Radix, Allocator>::to_string
(
) const
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    if ( this->digits_.empty() )
    {
        return std::string( "0" );
    }
    else
    {
        std::string           result;
        char                  digit_scratch_space[ 2 + std::numeric_limits<
                                                        digit_type>::digits10 ];
        char const * const    separator = detail::get_digit_separator_string<
                                                                       Radix>();
        const_reverse_iterator        b = this->digits_.rbegin();
        const_reverse_iterator const  e = this->digits_.rend();

        result.reserve( this->digits_.size() * sizeof(digit_scratch_space) );
        goto place_digit;

    place_separator:
        result.append( separator );

    place_digit:
        detail::write_integer_digits( *b, digit_scratch_space );
        result.append( digit_scratch_space );

        if ( e != ++b )
        {
            goto place_separator;
        }

    loop_finished:
        result.reserve();
        return result;
    }
}

/** Returns the value of the current object as a user-supplied type.

    \return  The object's equivalent value

    \throws boost::math::big_radix_whole_conversion_error
             The current value is larger than what the receiving type can store.
 */
template < int Radix, class Allocator >
template < typename NumericType >
inline
NumericType
boost::math::big_radix_whole<Radix, Allocator>::to_number
(
) const
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    return detail::convert_digit_string_to_number( this->digits_.rbegin(),
     this->digits_.rend(), static_cast<NumericType>(self_type::radix),
     big_radix_whole_conversion_error("value too large for converted type") );
}


#endif  // BOOST_MATH_BIG_RADIX_WHOLE_IMPL_CONVERSION_IPP

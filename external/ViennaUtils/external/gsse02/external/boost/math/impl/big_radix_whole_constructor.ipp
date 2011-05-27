//  Boost math/impl/big_radix_whole_constructor.ipp header file  -------------//

//  Copyright 2006 Daryle Walker.  Distributed under the Boost Software License,
//  Version 1.0.  (See the accompanying file LICENSE_1_0.txt or a copy at
//  <http://www.boost.org/LICENSE_1_0.txt>.)

//  See <http://www.boost.org/libs/math/> for the library's home page.

/** \file
    \brief  Part of the implementation for an arbitrary-length specific-radix
            unsigned integer class template

    Contains the definitions some of the public member functions of the
    \c boost::math::big_radix_whole\<\> class template.  These member functions
    are the constructors.
 */

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_IMPL_CONSTRUCTOR_IPP
#define BOOST_MATH_BIG_RADIX_WHOLE_IMPL_CONSTRUCTOR_IPP

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_CORE_HPP
#error "#Include only as part of <boost/math/big_radix_whole_core.hpp>"
#endif

#include <algorithm>  // for std::find_if
#include <cmath>      // PROVISIONAL, for std::log, ceil
#include <cstdlib>    // for std::div, div_t
#include <iterator>   // for std::back_inserter
#include <limits>     // for std::numeric_limits
#include <string>     // for std::string

#include <boost/assert.hpp>         // for BOOST_ASSERT
#include <boost/lambda/lambda.hpp>  // for boost::lamda::_1
#include <boost/static_assert.hpp>  // for BOOST_STATIC_ASSERT


//! \cond
//  Implementation details  --------------------------------------------------//

namespace boost
{
namespace math
{
namespace detail
{

// Collapse the (last few) digits of a list to a single digit value, with a
// given radix separating the digits to be fused.
template < class Exception, class DequeType >
void
collapse_digit_list
(
    DequeType &                     list,
    typename DequeType::size_type   span,
    typename DequeType::value_type  radix
)
{
    typedef typename DequeType::value_type     digit_type;
    typedef ::std::numeric_limits<digit_type>  digit_limits;

    BOOST_STATIC_ASSERT( digit_limits::is_specialized );
    BOOST_STATIC_ASSERT( digit_limits::is_bounded );

    digit_type  value = 0;
    bool        did_loop = false;

    // Loop through the digits to be replaced
    for ( digit_type multiplier = 1 ; !list.empty() && span ; --span, multiplier
     *= radix, list.pop_front(), did_loop = true )
    {
        digit_type const  next_digit = list.front();

        if ( (digit_limits::max() - value) / multiplier < next_digit )
        {
            throw Exception( "resulting digit is too large" );
        }
        else
        {
            value += next_digit * multiplier;
        }
    }

    // Commit the replacement digit
    if ( did_loop )
    {
        list.push_front( value );
    }
}

// Convert a string of digit characters, with a possible sign, to a list of
// digit values.  The string may begin with a "+" or '-'.  The digits are 0-9,
// A-Z, and a-z, representing values 0 through 35.  If there are hyphens within
// the string, then each hyphen-separated group is taken as a huge digit with a
// decimal representation.  Otherwise, each digit character is individually
// taken as a separate digit in the final list.
template < class Exception, class DequeType >
bool
convert_string_to_digits
(
    ::std::string const &  s,
    DequeType &            destination
)
{
    using ::std::string;

    bool  is_negative = false;

    // Skip empty strings
    if ( !s.empty() )
    {
        string::const_iterator        b( s.begin() );
        string::const_iterator const  e( s.end() );

        // Consider a leading sign character
        switch ( *b++ )
        {
        case '+':
            is_negative = false;  // positive
            break;

        case '-':
            is_negative = true;  // negative
            break;

        default:
            --b;  // undo incrementing past the non-existant sign
            break;
        }

        // Loop through the characters, measuring the spans
        // between any separating hyphens (if found)
        typename DequeType::size_type  span = 0u;
        bool                           grouped = false;

        for ( ; e != b ; ++b )
        {
            int  value = 0;

            // TODO: should I replace this with a search/find through
            // strings with the numbers/letters in order?
            switch ( *b )
            {
            case 'Z': case 'z': ++value;  // FALL THROUGH
            case 'Y': case 'y': ++value;  // FALL THROUGH
            case 'X': case 'x': ++value;  // FALL THROUGH
            case 'W': case 'w': ++value;  // FALL THROUGH
            case 'V': case 'v': ++value;  // FALL THROUGH
            case 'U': case 'u': ++value;  // FALL THROUGH
            case 'T': case 't': ++value;  // FALL THROUGH
            case 'S': case 's': ++value;  // FALL THROUGH
            case 'R': case 'r': ++value;  // FALL THROUGH
            case 'Q': case 'q': ++value;  // FALL THROUGH
            case 'P': case 'p': ++value;  // FALL THROUGH
            case 'O': case 'o': ++value;  // FALL THROUGH
            case 'N': case 'n': ++value;  // FALL THROUGH
            case 'M': case 'm': ++value;  // FALL THROUGH
            case 'L': case 'l': ++value;  // FALL THROUGH
            case 'K': case 'k': ++value;  // FALL THROUGH
            case 'J': case 'j': ++value;  // FALL THROUGH
            case 'I': case 'i': ++value;  // FALL THROUGH
            case 'H': case 'h': ++value;  // FALL THROUGH
            case 'G': case 'g': ++value;  // FALL THROUGH
            case 'F': case 'f': ++value;  // FALL THROUGH
            case 'E': case 'e': ++value;  // FALL THROUGH
            case 'D': case 'd': ++value;  // FALL THROUGH
            case 'C': case 'c': ++value;  // FALL THROUGH
            case 'B': case 'b': ++value;  // FALL THROUGH
            case 'A': case 'a': ++value;  // FALL THROUGH
            case '9': ++value;  // FALL THROUGH
            case '8': ++value;  // FALL THROUGH
            case '7': ++value;  // FALL THROUGH
            case '6': ++value;  // FALL THROUGH
            case '5': ++value;  // FALL THROUGH
            case '4': ++value;  // FALL THROUGH
            case '3': ++value;  // FALL THROUGH
            case '2': ++value;  // FALL THROUGH
            case '1': ++value;  // FALL THROUGH
            case '0':
                destination.push_front( value );
                ++span;
                break;
            case '-':
                if ( span )
                {
                    // Fuse the last span of digits to one huge digit
                    collapse_digit_list<Exception>( destination, span, 10 );
                    span = 0;
                    grouped = true;
                    break;
                }
                else
                {
                    // No digit characters between the last two hyphens
                    throw Exception( "no number between digit separators" );
                }
            default:
                throw Exception( "illegal character for number format" );
            }
        }

        // Take care of fusing the last hypen-group of characters
        if ( grouped )
        {
            if ( span )
            {
                collapse_digit_list<Exception>( destination, span, 10 );
            }
            else
            {
                // string ended with a trailing hyphen
                throw Exception( "no number after last hyphen" );
            }
        }
    }

    return is_negative;
}

// Copy the digits of a number towards an output iterator, lowest places first
template < int Rx, class Al, typename OutputIterator >
OutputIterator
copy_big_radix_whole_digits
(
    big_radix_whole<Rx, Al> const &  source,
    OutputIterator                   destination
)
{
    typedef typename big_radix_whole<Rx, Al>::size_type  size_type;

    size_type const  s = source.digit_count();

    for ( size_type  i = 0u ; s > i ; ++i )
    {
        *destination++ = source.quick_digit_at( i );
    }

    return destination;
}

// Create digits, lowest places first, from a digit string of a larger radix
template < int RadixS, int RadixD, class DequeType, typename OutputIterator >
OutputIterator
copy_big_radix_whole_to_smaller_radix
(
    DequeType &     source,
    OutputIterator  destination
)
{
    BOOST_STATIC_ASSERT( RadixS > RadixD && RadixD >= 2 );

    typedef typename DequeType::reverse_iterator  iterator;

    // The elements change value, but the length won't change
    iterator const  b = source.rbegin(), e = source.rend();

    // Keep going until the source represents zero
    while ( ::std::find_if(b, e, lambda::_1 != 0) != e )
    {
        // This is copied from big_radix_whole::div_and_mod_single
        ::std::div_t  qr = { 0, 0 };

        for ( iterator  i = b ; e != i ; ++i )
        {
            typename DequeType::reference  digit( *i );

            qr = ::std::div( qr.rem * RadixS + digit, RadixD );
            digit = qr.quot;
        }

        *destination++ = qr.rem;
    }

    return destination;
}

// Create digits, from a digit string of a smaller radix
template < int RadixD, int RadixS, class Al, class DequeType,
 typename Exception >
void
copy_big_radix_whole_to_larger_radix
(
    big_radix_whole<RadixS, Al> const &  source,
    DequeType &                          destination,
    Exception const &                    error
)
{
    using ::std::log;

    BOOST_STATIC_ASSERT( RadixS < RadixD && RadixS >= 2 );
    BOOST_ASSERT( destination.empty() );

    typedef typename big_radix_whole<RadixS, Al>::size_type  size_type;
    typedef typename DequeType::iterator                     iterator;

    // Allocate all the scratch space in advance.  Since the destination radix
    // is larger, the number of digits needed is never smaller, so there is an
    // easy-to-find upper bound (the number of source digits).  But we reduce
    // the space allocated by recognizing groupings by powers (like ten base-3
    // digits can become five base-9 digits since 3**2 == 9).
    size_type const  ss = source.digit_count();
    double const     sd = 1.0 + ::std::ceil(static_cast<double>( ss ) *
     log( static_cast<double>(RadixS) ) / log( static_cast<double>(RadixD) ));

    BOOST_ASSERT( sd >= ss );
    if ( destination.max_size() < sd )
    {
        throw error;
    }
    destination.insert( destination.begin(), static_cast<typename
     DequeType::size_type>(sd), 0 );

    // Multiply-and-add through each digit, higher places first
    iterator const  b = destination.begin(), e = destination.end();

    for ( size_type  i = ss ; 0 < i ; --i )
    {
        // This is copied from big_radix_whole::multiply_single_add_single
        ::std::div_t  qr;

        qr.quot = source.quick_digit_at( i - 1u );

        for ( iterator  j = b ; e != j ; ++j )
        {
            typename DequeType::reference  digit( *j );

            qr = ::std::div( digit * RadixS + qr.quot, RadixD );
            digit = qr.rem;
        }
        BOOST_ASSERT( 0 == qr.quot );  // highest place shouldn't make carry
    }

    // Remove the extra leading zeros
    while ( !destination.empty() && !destination.back() )
    {
        destination.pop_back();
    }
}

// Convert between radices, but encapsulate their relative order at compile-time
// (This general version is used for unequal radices.)
template < int RxS, int RxD >
class big_radix_whole_radix_converter
{
    // Use specialization to distinguish between strictly-less-than
    // vs. strictly-greater-than for the two radices.
    // (This general version is used for strictly less-than.)
    template < int RxSS, int RxDD, bool ToHigherRadix >
    class radix_converter
    {
    public:
        template < class Al, class DequeType, typename Exception >
        void  operator ()
        (
            big_radix_whole<RxSS, Al> const &  source,
            DequeType &                        destination,
            Exception const &                  error
        )
        {
            copy_big_radix_whole_to_larger_radix<RxDD>( source, destination,
             error );
        }

    };

    // (Partial) specialization for (strictly) greater-than
    template < int RxSS, int RxDD >
    class radix_converter< RxSS, RxDD, false >
    {
    public:
        template < class Al, class DequeType, typename Exception >
        void  operator ()
        (
            big_radix_whole<RxSS, Al> const &  source,
            DequeType &                        destination,
            Exception const &                  // "error" is unused
        )
        {
            using ::std::back_inserter;

            DequeType  copy( destination.get_allocator() );

            copy_big_radix_whole_digits( source, back_inserter(copy) );
            copy_big_radix_whole_to_smaller_radix<RxSS, RxDD>( copy,
             back_inserter(destination) );
        }

    };

public:
    template < class Al, class DequeType, typename Exception >
    void  operator ()
    (
        big_radix_whole<RxS, Al> const &  source,
        DequeType &                       destination,
        Exception const &                 error
    )
    {
        radix_converter<RxS, RxD, (RxS < RxD)>()( source, destination, error );
    }

};  // boost::math::detail::big_radix_whole_radix_converter (1)

// (Partial) specialization for equal radices
template < int Rx >
class big_radix_whole_radix_converter< Rx, Rx >
{
public:
    template < class Al, class DequeType, typename Exception >
    void  operator ()
    (
        big_radix_whole<Rx, Al> const &  source,
        DequeType &                      destination,
        Exception const &                // "error" is unused
    )
    {
        copy_big_radix_whole_digits(source, ::std::back_inserter(destination));
    }

};  // boost::math::detail::big_radix_whole_radix_converter (2)

}  // namespace detail
}  // namespace math
}  // namespace boost
//! \endcond


#include <boost/concept_check.hpp>  // for boost::function_requires, etc.
#include <boost/cstdint.hpp>        // for boost::uintmax_t


//  Radix/bignum/natural constructor definitions  ----------------------------//

/** Constructs a \c big_radix_whole\<\> object with given initial value and
    allocator.  It also serves as a conversion constructor from built-in
    unsigned integer types and as a default constructor.

    \param value      The initial numerical value of this object.  If not given,
                      it defaults to zero.
    \param allocator  The object to be used for any memory allocations or
                      deallocations needed.  If not given, it defaults to a
                      default-constructed \c allocator_type object.

    \post  <code><var>value</var> == <b>Sum</b>{<var>i</var>}(
           Radix<sup><var>i</var></sup> * this-&gt;digit_at(<var>i</var>)
           )</code>
    \post  <code><var>allocator</var> == this-&gt;get_allocator()</code>
 */
template < int Radix, class Allocator >
boost::math::big_radix_whole<Radix, Allocator>::big_radix_whole
(
    uintmax_t               value,     // = 0u
    allocator_type const &  allocator  // = allocator_type()
)
    : digits_( allocator )
{
    for ( uintmax_t const  r = static_cast<uintmax_t>(self_type::radix) ; value
     ; value /= r )
    {
        this->digits_.push_back( static_cast<digit_type>(value % r) );
    }
    BOOST_ASSERT( this->test_invariant() );
}

/** Constructs a \c big_radix_whole\<\> object from a list of digits and an
    allocator.  The list will be represented by a begin-end iterator pair, with
    the list starting from the least-significant digit up through the highest.

    \pre  \p InputIterator models the input-iterator concept (at least).

    \param begin      An iterator to the first element of the digit list.  That
                      starting digit represents the one's place of the number,
                      with higher places occurring in order.  If the list is
                      empty, then this argument must equal the \p end one.
    \param end        An iterator to one-past-the-end of the digit list.  It
                      must be reachable from \p begin through a finite number of
                      increments.
    \param allocator  The object to be used for any memory allocations or
                      deallocations needed.  If not given, it defaults to a
                      default-constructed \c allocator_type object.

    \post  (No easily-expressed equation of the value based on <var>begin</var>
           and <var>end</var>.)
    \post  <code><var>allocator</var> == this-&gt;get_allocator()</code>
 */
template < int Radix, class Allocator >
template < typename InputIterator >
inline
boost::math::big_radix_whole<Radix, Allocator>::big_radix_whole
(
    InputIterator           begin,
    InputIterator           end,
    allocator_type const &  allocator  // = allocator_type()
)
    : digits_( begin, end, allocator )
{
  //    Function_requires< ::boost::InputIterator<InputIterator> >();
    this->normalize();
    BOOST_ASSERT( this->test_invariant() );
}

/** Constructs a \c big_radix_whole\<\> object from a number encoded as a string
    of text and from an allocator.  It has a format like:
    "(\+|-)?[[:alnum:]]+(-[[:alnum:]]+)*"

    \param digit_text  The string to be interpreted as a number.  The number
                       must take up the entire string.  It may begin with a '+'
                       or '-' sign.  The actual digits are 0-9, a-z, or A-Z,
                       with letters representing values 10-35.  If there are no
                       hyphens within the string (not including an initial '-'),
                       then each digit is translated into a digit for the final
                       number.  Otherwise, each hyphen-separated group is taken
                       as a decimal number, and the group's value is used as a
                       digit for the final number.  Empty groups are forbidden.
                       The digits at the beginning of the string are the highest
                       places.  An empty string is interpreted as zero.  Any
                       other character (including a '+' outside the first
                       position) is an error.
    \param allocator   The object to be used for any memory allocations or
                       deallocations needed.  If not given, it defaults to a
                       default-constructed \c allocator_type object.

    \throws boost::math::big_radix_whole_bad_format_error
             The <var>digit_text</var> string has an improper format.
    \throws boost::math::big_radix_whole_negative_result_error
             The <var>digit_text</var> string describes a value less than zero.
             A negative zero is acceptable, but no other negative value is.

    \post  (No easily-expressed equation of the value based on
           <var>digit_text</var>.)
    \post  <code><var>allocator</var> == this-&gt;get_allocator()</code>
 */
template < int Radix, class Allocator >
boost::math::big_radix_whole<Radix, Allocator>::big_radix_whole
(
    std::string const &     digit_text,
    allocator_type const &  allocator  // = allocator_type()
)
    : digits_( allocator )
{
    if ( detail::convert_string_to_digits<big_radix_whole_bad_format_error>(
     digit_text, this->digits_) )
    {
        // Got here only if the string indicated a negative number.  Since this
        // type only supports non-negative values, any mantissa value besides
        // zero is an error.
        if ( std::find_if(this->digits_.begin(), this->digits_.end(), lambda::_1
         != 0) != this->digits_.end() )
        {
            throw big_radix_whole_negative_result_error("negative value read");
        }
    }
    this->normalize();
    BOOST_ASSERT( this->test_invariant() );
}

/** Constructs a \c big_radix_whole\<\> object from an object of a different
    version of the \c big_radix_whole template and an allocator.

    \param source     The object that is the source of the initial numerical
                      value.
    \param allocator  The object to be used for any memory allocations or
                      deallocations needed.  If not given, it defaults to a
                      default-constructed \c allocator_type object.

    \post  <code><var>source</var> == *this</code>
    \post  <code><var>allocator</var> == this-&gt;get_allocator()</code>
 */
template < int Radix, class Allocator >
template < int Radix2, class Allocator2 >
boost::math::big_radix_whole<Radix, Allocator>::big_radix_whole
(
    big_radix_whole<Radix2, Allocator2> const &  source,
    allocator_type const &                       allocator  // =allocator_type()
)
    : digits_( allocator )
{
    detail::big_radix_whole_radix_converter<Radix2, Radix>()( source,
     this->digits_, big_radix_whole_conversion_error("not enough allocation "
     "space for digits") );

    BOOST_ASSERT( this->test_invariant() );
}


#endif  // BOOST_MATH_BIG_RADIX_WHOLE_IMPL_CONSTRUCTOR_IPP

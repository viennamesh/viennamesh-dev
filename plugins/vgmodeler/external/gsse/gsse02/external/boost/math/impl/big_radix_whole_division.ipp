//  Boost math/impl/big_radix_whole_division.ipp header file  ----------------//

//  Copyright 2006 Daryle Walker.  Distributed under the Boost Software License,
//  Version 1.0.  (See the accompanying file LICENSE_1_0.txt or a copy at
//  <http://www.boost.org/LICENSE_1_0.txt>.)

//  See <http://www.boost.org/libs/math/> for the library's home page.

/** \file
    \brief  Part of the implementation for an arbitrary-length specific-radix
            unsigned integer class template

    Contains the definitions some of the public member functions of the
    \c boost::math::big_radix_whole\<\> class template, and some related
    non-member functions.  All of these (member) functions perform various
    divisions.
 */

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_IMPL_DIVISION_IPP
#define BOOST_MATH_BIG_RADIX_WHOLE_IMPL_DIVISION_IPP

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_CORE_HPP
#error "#Include only as part of <boost/math/big_radix_whole_core.hpp>"
#endif

#include <algorithm>  // for std::count_if, min

#include <boost/lambda/lambda.hpp>  // for boost::lamda::_1


//! \cond
//  Implementation details  --------------------------------------------------//

namespace boost
{
namespace math
{
namespace detail
{

// Determine the even/odd status of a digit string.  The method used is
// dependent on the radix.
template < int Radix >
class big_radix_whole_parity_checker
{
    // The method used depends on the even/odd status of the radix
    // (Useless "Rx" parameter added since the compiler needs it.)
    template < int Rx, bool IsOdd >
    class parity_checker
    {
    public:
        template < class DequeType >
        int  operator ()( DequeType const &digits ) const
        {
            // Odd * Any == Any; Odd + Any == ~Any; Even + Any == Any
            // This means that the even/odd status of a particular place depends
            // on the even/odd status of the place's digit.  All those statuses
            // have to be added together for the final status; so a number is
            // odd only if there is an odd number of odd place digits.
            return ::std::count_if( digits.begin(), digits.end(), (lambda::_1 &
             1) != 0 ) % 2;
        }

    };  // parity_checker (1)

    // The method for even radices is quicker
    // (Useless "Rx" parameter added since the compiler can't accept an inner
    // template that's fully specialized.)
    template < int Rx >
    class parity_checker< Rx, false >
    {
    public:
        template < class DequeType >
        int  operator ()( DequeType const &digits ) const
        {
            // Even * Any == Even; Even + Any == Any
            // This means that all the places above the one's place contribute
            // an even number, since the radix (and therefore all its non-zero
            // powers) is even.  The even/odd attribute is depedent solely on
            // whether the one's place is even/odd.
            return digits.empty() ? 0 : ( digits.front() & 1 );
        }

    };  // parity_checker (2)

public:
    template < class DequeType >
    int  operator ()( DequeType const &digits ) const
    {  return parity_checker<Radix, Radix % 2 != 0>()( digits );  }

};  // boost::math::detail::big_radix_whole_parity_checker (1)

// Base-2 is really easy to specialize; little computation needed
template < >
class big_radix_whole_parity_checker< 2 >
{
public:
    template < class DequeType >
    int  operator ()( DequeType const &digits ) const
    {
        // Unlike other even radices, the one's digit is already either 0 or 1
        return digits.empty() ? 0 : digits.front();
    }

};  // boost::math::detail::big_radix_whole_parity_checker (2)

}  // namespace detail
}  // namespace math
}  // namespace boost
//! \endcond


#include <cstdlib>  // for std::div, div_t

#include <boost/assert.hpp>  // for BOOST_ASSERT


//  Radix/bignum/natural extra member function definitions  ------------------//

/** Attenuates the current number by a given positive value less than the radix.
    It should be faster than converting the short divisor to a full
    \c big_radix_whole\<\> object before dividing.

    \pre  <code>0 &lt; <var>divisor</var> &lt; Radix</code>

    \param divisor  The value that the current object will be divided by

    \return  The remainder after division

    \throws boost::math::big_radix_whole_divide_by_zero_error
             A zero-valued divisor is used.

    \post  <code><var>old_this</var> == *this * <var>divisor</var> +
           <var>return_value</var></code>
 */
template < int Radix, class Allocator >
typename boost::math::big_radix_whole<Radix, Allocator>::digit_type
boost::math::big_radix_whole<Radix, Allocator>::div_and_mod_single
(
    digit_type  divisor
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    BOOST_ASSERT( 0 <= divisor && divisor < self_type::radix );

    if ( !divisor )
    {
        // something / 0 -> banned
        throw big_radix_whole_divide_by_zero_error( "attempted to short-divide"
         " by zero" );
    }

    // ELSE: perform the short division, working even if the dividend is zero
    std::div_t             qr = { 0, 0 };
    reverse_iterator const  e = this->digits_.rend();

    for ( reverse_iterator  i = this->digits_.rbegin() ; e != i ; ++i )
    {
        qr = std::div( qr.rem * self_type::radix + *i, divisor );
        *i = qr.quot;
    }

    this->clear_leading_zeros();

    BOOST_ASSERT( this->test_invariant() );
    return qr.rem;
}

/** Gets the resulting remainder if the current number was divided by a given
    positive value less than the radix.  It should be faster than using the full
    short-division routine #div_and_mod_single(digit_type) since this member
    function does not have to write back to, or otherwise change, the allocated
    digits for the dividend.

    \pre  <code>0 &lt; <var>divisor</var> &lt; Radix</code>

    \param divisor  The value that the current object will be divided by

    \return  The remainder after division

    \throws boost::math::big_radix_whole_divide_by_zero_error
             A zero-valued divisor is used.
 */
template < int Radix, class Allocator >
typename boost::math::big_radix_whole<Radix, Allocator>::digit_type
boost::math::big_radix_whole<Radix, Allocator>::modulo_single
(
    digit_type  divisor
) const
{

    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    BOOST_ASSERT( 0 <= divisor && divisor < self_type::radix );

    if ( !divisor )
    {
        // something % 0 -> banned
        throw big_radix_whole_divide_by_zero_error( "attempted to short-modulo"
         " by zero" );
    }

    // ELSE: perform the short division, working even if the dividend is zero
    digit_type            remainder = 0;
    const_reverse_iterator const  e = this->digits_.rend();

    for ( const_reverse_iterator  i = this->digits_.rbegin() ; e != i ; ++i )
    {
        // Shift-in and divide the next dividend digit
        remainder *= self_type::radix;
        remainder += *i;
        remainder %= divisor;
    }

    return remainder;
}

/** Gets the current number's quality of being either even or odd.  This is the
    same as getting a modulo by two.  It should be faster than general division,
    modulo, or shift routines because this member function can take advantage of
    bit twiddling.

    \retval 0  <code>*this</code> is even.
    \retval 1  <code>*this</code> is odd.
 */
template < int Radix, class Allocator >
inline
int
boost::math::big_radix_whole<Radix, Allocator>::parity
(
) const
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    return detail::big_radix_whole_parity_checker<Radix>()( this->digits_ );
}

/** Gets whether or not the current number is odd (i.e. its remainder modulo two
    is one).

    \retval true   <code>*this</code> is odd.
    \retval false  <code>*this</code> is even.
 */
template < int Radix, class Allocator >
inline
bool
boost::math::big_radix_whole<Radix, Allocator>::is_odd
(
) const
{
    return static_cast<bool>( this->parity() );
}

/** Gets whether or not the current number is even (i.e. its remainder modulo
    two is zero).

    \retval true   <code>*this</code> is even.
    \retval false  <code>*this</code> is odd.
 */
template < int Radix, class Allocator >
inline
bool
boost::math::big_radix_whole<Radix, Allocator>::is_even
(
) const
{
    return !this->is_odd();
}

/** Partitions the current number in blocks sized by a given value, possibly
    with unpartitioned leftovers.

    \pre  <code>0 != <var>divisor</var></code>

    \param divisor  The value that the current object, acting as the dividend,
                    will be divided by

    \return  An object <var>x</var> such that <code><var>x</var>.first</code> is
             the quotient from the division and <code><var>x</var>.second</code>
             is the division's remainder.

    \throws boost::math::big_radix_whole_divide_by_zero_error
             A zero-valued divisor is used.

    \post  <code>*this == <var>divisor</var> * <var>return_value</var>.first +
           <var>return_value</var>.second</code>
 */
template < int Radix, class Allocator >
std::pair< boost::math::big_radix_whole<Radix, Allocator>,
 boost::math::big_radix_whole<Radix, Allocator> >
boost::math::big_radix_whole<Radix, Allocator>::divide_by
(
    big_radix_whole const &  divisor
) const
{
//   std::cout << "modulo operator.. " << std::endl;

    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );
    BOOST_PRIVATE_WILD_ASSERT( divisor.test_invariant() );

    std::pair<self_type, self_type>  result( self_type(0u,
                                      this->get_allocator()), self_type(0u,
                                      this->get_allocator()) );
    self_type                        &q = result.first, &r = result.second;
    deque_type const &              drd = divisor.digits_;

    switch ( size_type const  drs = drd.size() )
    {
    default:
    {
        // multi-digit long division
        deque_type const &  ddd = this->digits_;
        size_type           dds = ddd.size();
        digit_type const    tdr = drd.back();

        while ( dds-- )
        {
            // The remainder acts as the scratch dividend
            r.shift_up_add_single( ddd[dds] );

            // Determine the trial quotient digit
            digit_type  tq = std::min<digit_type>( r.digit_at(drs) *
                         self_type::radix + r.digit_at(drs - 1u) / tdr,
                         self_type::radix - 1 );

            if ( r.subtract_mixed_product_absolutely(divisor, tq) )
            {
                // Undo the over-estimation
                do
                {
                    --tq;
                }
                while ( !r.subtract_full_absolutely(divisor) && r );
                    // "&& r" just in case re-estimate fully cancels
            }
            BOOST_ASSERT( r < divisor );

            // Incorporate that trial digit
            BOOST_ASSERT( 0 <= tq );  // already have "td < Radix"
            q.shift_up_add_single( tq );
        }
        return result;
    }

    case 1u:
        // single-digit division
        q.assign( *this );
        r.assign( static_cast<uintmax_t>(q.div_and_mod_single( drd.front() )) );
        return result;

    case 0u:
        // something / 0 -> banned
        throw big_radix_whole_divide_by_zero_error( "attempted to divide and/or"
         " modulo by zero" );
    }
}


//  Radix/bignum/natural operator member function definitions  ---------------//

/** Attenuates the current number by a given value, truncating away any
    fractional part.

    \pre  <code>0 != <var>divisor</var></code>

    \param divisor  The value to be divided into the current number, which
                    serves as the dividend.

    \return  A reference to <code>*this</code> object as the quotient.

    \throws boost::math::big_radix_whole_divide_by_zero_error
             A zero-valued divisor is used.

    \post  <code>*this == <b>Floor</b>( <var>old_this</var> <b>/</b>
           <var>divisor</var> )</code>

    \note  The class declaration uses the Boost.Operators library to synthesize
           the division <code>operator /</code> from this one.

    \see  #divide_by
 */
template < int Radix, class Allocator >
boost::math::big_radix_whole<Radix, Allocator> &
boost::math::big_radix_whole<Radix, Allocator>::operator /=
(
    big_radix_whole const &  divisor
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );
    BOOST_PRIVATE_WILD_ASSERT( divisor.test_invariant() );

    // For now, use the long division method.  Later, maybe something "kewl"
    // like multiplication by pseudo-reciprocal via Newton's method can be used.
    this->divide_by( divisor ).first.swap( *this );

    BOOST_ASSERT( this->test_invariant() );
    return *this;
}

/** Gets the resulting remainder if the current number was divided by a given
    value.

    \pre  <code>0 != <var>divisor</var></code>

    \param divisor  The value to be divided into the current number, which
                    serves as the dividend.

    \return  A reference to <code>*this</code> object as the remainder.

    \throws boost::math::big_radix_whole_divide_by_zero_error
             A zero-valued divisor is used.

    \post  <code>*this == <var>old_this</var> - <var>divisor</var> *
           <b>Floor</b>( <var>old_this</var> / <var>divisor</var> )</code>

    \note  The class declaration uses the Boost.Operators library to synthesize
           the modulus <code>operator %</code> from this one.

    \see  #divide_by
 */
template < int Radix, class Allocator >
boost::math::big_radix_whole<Radix, Allocator> &
boost::math::big_radix_whole<Radix, Allocator>::operator %=
(
    big_radix_whole const &  divisor
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );
    BOOST_PRIVATE_WILD_ASSERT( divisor.test_invariant() );

    // For now, use the long division method.  Later, maybe something "kewl"
    // like an exact-remainder method can be used.
//     std::cout << "### divisor: " <<  this->divide_by( divisor ).second << std::endl;

    this->divide_by( divisor ).second.swap( *this );

    BOOST_ASSERT( this->test_invariant() );
    return *this;
}


//  Radix/bignum/natural miscellaneous function definitions  -----------------//

// Put something here?


#endif  // BOOST_MATH_BIG_RADIX_WHOLE_IMPL_DIVISION_IPP

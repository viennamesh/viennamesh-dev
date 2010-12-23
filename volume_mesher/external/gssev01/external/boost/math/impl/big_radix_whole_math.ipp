//  Boost math/impl/big_radix_whole_math.ipp header file  --------------------//

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
    common mathematical integer functions.
 */

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_IMPL_MATH_IPP
#define BOOST_MATH_BIG_RADIX_WHOLE_IMPL_MATH_IPP

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_CORE_HPP
#error "#Include only as part of <boost/math/big_radix_whole_core.hpp>"
#endif

#include <cstddef>  // for std::size_t

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

// Determine how many digits a particular number would need.
template < int Radix >
std::size_t  digits_for_number( uintmax_t value )
{
    BOOST_STATIC_ASSERT( Radix > 1 );

    std::size_t  result = 0u;

    for ( uintmax_t const  r = static_cast<uintmax_t>(Radix) ; value ; value /=
     r )
    {
        ++result;
    }
    return result;
}

}  // namespace detail
}  // namespace math
}  // namespace boost
//! \endcond


#include <algorithm>  // for std::count, min, reverse
#include <climits>    // for INT_MAX
#include <cstdlib>    // for std::div_t, div
#include <numeric>    // for std::accumulate

#include <boost/assert.hpp>  // for BOOST_ASSERT


//  Radix/bignum/natural extra member function definitions  ------------------//

/** Replaces the current value with its absolute value.

    \post  <code>*this == |<var>old_this</var>|</code>

    \note  Since all the potential values this object can have are nonnegative,
           their absolute values map to themselves.  This means that this
           object's state never changes and no actual action is done.  So this
           member function can be <code>const</code>.
 */
template < int Radix, class Allocator >
inline
void
boost::math::big_radix_whole<Radix, Allocator>::absolve_self
(
) const
{
    // All values map to themselves.  This is the same mapping done in the
    // "identify_self" member function.
    this->identify_self();
}

/** Replaces the current value with its (arithmetic) sign.  That is the
    direction to the value's place in the number line relative to zero.  The
    sign can be zero (for a zero value) or one (for positive values).

    \post  <code>*this == |<var>old_this</var>| / <var>old_this</var></code> if
           <var>old_this</var> represented a non-zero value.
    \post  <code>*this == <var>old_this</var></code> if <var>old_this</var>
           represented zero.
 */
template < int Radix, class Allocator >
inline
void
boost::math::big_radix_whole<Radix, Allocator>::sign_self
(
)
{
    // Non-zero (i.e. positive) values map to one, while zero maps to itself.
    // This mapping is also done in the "verify_self" member function.
    this->verify_self();
}

/** Gets how many times a given value appears as a digit within the current
    number.  Since a number could be considered to have an infinite number of
    higher placed zeros, only zeros internal to the number's mantissa are
    counted.  This implies that the number zero will give a count of zero for
    the occurrences of the zero (or any other) digit.

    \pre  <code>0 &lt;= <var>value</var> &lt; Radix</code>

    \param value  The digit value to be checked

    \return  <code><b>Sum</b>{ 0 &lt;= <var>i</var> &lt; this-&gt;digit_count()
             }( static_cast&lt;size_type&gt;(<var>value</var> ==
             this-&gt;digit_at( <var>i</var> )) )</code>
 */
template < int Radix, class Allocator >
inline
typename boost::math::big_radix_whole<Radix, Allocator>::size_type
boost::math::big_radix_whole<Radix, Allocator>::specific_digit_count
(
    digit_type  value
) const
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    BOOST_ASSERT( 0 <= value && value < self_type::radix );

    return std::count( this->digits_.begin(), this->digits_.end(), value );
}

/** Replaces the current value with its digit sum.  Such a sum is produced by
    adding all of the value's digits together.  Single-digit and zero values are
    their own digit sum.

    \post  <code>*this == <b>Sum</b>{ <var>i</var> }(
           <var>old_this</var>.digit_at(<var>i</var>) )</code>
 */
template < int Radix, class Allocator >
void
boost::math::big_radix_whole<Radix, Allocator>::digit_summate_self
(
)
{
    using std::div;

    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    size_type const  s = this->digits_.size();

    if ( s >= 2u )
    {
        // The upper bound for the sum is "s * (Radix - 1)".  There is an easier
        // to compute higher bound of "s * Radix".  Use that to pre-allocate a
        // sufficient amount of digits.
        deque_type      mantissa( detail::digits_for_number<Radix>(s) + 1u,
                         digit_type(), this->digits_.get_allocator() );
        iterator const  mb_inset = mantissa.begin() + 1;
        iterator const  me_inset = mantissa.end() - 1;

        // Add as many digits with built-in arithmetic as possible
        int const     run_limit = INT_MAX / self_type::radix;
        const_iterator const  e = this->digits_.end();

        for ( const_iterator  i = this->digits_.begin() ; e > i ; )
        {
            const_iterator const  local_end = i + std::min<difference_type>( e -
             i, run_limit );

            int const  run_sum = std::accumulate( i, local_end, 0 );

            // Update the loop counter
            i = local_end;

            // Update the grand total with the local sum
            std::div_t  qr = div( run_sum, self_type::radix );
            iterator     j = mb_inset;

            mantissa[ 0u ] += qr.rem;
            mantissa[ 1u ] += qr.quot;

            qr = div( mantissa[0u], self_type::radix );
            mantissa[ 0u ] = qr.rem;
            mantissa[ 1u ] += qr.quot;

            while ( (me_inset > j) && (self_type::radix <= *j) )
            {
                reference  digit = *j;

                qr = div( digit, self_type::radix );
                digit = qr.rem;
                *++j = qr.quot;
            }
            BOOST_ASSERT( mantissa.back() < self_type::radix );
        }

        // Use the sum as the new number
        this->digits_.swap( mantissa );
        this->clear_leading_zeros();
    }
    // ELSE: single-digits are their own digit-sum -> no change

    BOOST_ASSERT( this->test_invariant() );
}

/** Gets the single-digit value resulting from obtaining the digit sum of the
    current number and then repeatedly digit-summating the previous sum.  (The
    terminating case is that single-digit and zero values are their own digit
    sum.)  The computation uses a method faster than explicitly doing repeated
    digit-summation.

    \retval *this                            <code>*this &lt; Radix</code>
    \retval digit_sum(*this).digital_root()  otherwise

    \see  boost::math::digit_sum(big_radix_whole const&)
 */
template < int Radix, class Allocator >
typename boost::math::big_radix_whole<Radix, Allocator>::digit_type
boost::math::big_radix_whole<Radix, Allocator>::digital_root
(
) const
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    switch ( size_type  s = this->digits_.size() )
    {
    default:
    {
        // The algorithm is called "Ramans' Formula" by Wikipedia.  It is:
        // dr(n) = either: n % (Radix - 1)  if: n != 0 [mod (Radix - 1)]
        //             or: Radix - 1        if: n == 0 [mod (Radix - 1)]
        //       = (n - 1) % (Radix - 1) + 1
        // (This code is adapted from the "modulo_single" member function.)
        digit_type      remainder = 0;
        digit_type const  divisor = self_type::radix - 1;

        do
        {
            // Shift-in and divide the next dividend digit
            remainder *= self_type::radix;
            remainder += this->digits_[ --s ];
            remainder %= divisor;
        }
        while ( s );

        // Like a clock's modulo mapping 0 to 12, wrap any zero result here
        return remainder ? remainder : divisor;
    }

    case 1u:
        // Single digits are their own digital root (and digit sum)
        return this->digits_.front();

    case 0u:
        // Same as the single-digit case, but there's no stored digit
        return 0;
    }
}

/** Changes the current number so its digits are in reversed order.  Since any
    leading zeros are purged from the number's state, a number with trailing
    zeros will not be recoverable by a second reversal (e.g. 45 goes to 54,
    which can go back to 45; but 360 goes to 63, which goes to 36).
    Single-digit and zero values are their own reversal.

    \post  For any <var>i</var>: <code><var>old_this</var>.digit_at(
           <var>i</var> ) == this-&gt;digit_at(
           <var>old_this</var>.digit_count() - 1 - <var>i</var> )</code>.
 */
template < int Radix, class Allocator >
inline
void
boost::math::big_radix_whole<Radix, Allocator>::reverse_digits
(
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    // Reverse and purge (legal trailing zeros become illegal leading zeros)
    std::reverse( this->digits_.begin(), this->digits_.end() );
    this->clear_leading_zeros();

    BOOST_ASSERT( this->test_invariant() );
}


//  Radix/bignum/natural miscellaneous function definitions  -----------------//

/** \brief  Absolute-value function for \c big_radix_whole\<\>

    Computes the absolute value for the given \c big_radix_whole\<\> object.
    The returned value is the distance between the origin and the given value's
    position on the number line, regardless of direction.  Since all objects of
    the \c big_radix_whole\<\> type represent nonnegative values, this function
    returns its argument.

    \param x  The function's argument.

    \return  \p x

    \see  boost::math::big_radix_whole::absolve_self

    \relates  boost::math::big_radix_whole
 */
template < int Radix, class Allocator >
inline
boost::math::big_radix_whole<Radix, Allocator>
boost::math::abs
(
    boost::math::big_radix_whole<Radix, Allocator> const &  x
)
{
    return x;
}

/** \brief  (Arithmetic) sign function for \c big_radix_whole\<\>

    Computes the sign for the given \c big_radix_whole\<\> object, i.e. the
    signum function.  This is the direction the object's value is on the number
    line, relative to the origin.  Since all objects of the
    \c big_radix_whole\<\> type represent nonnegative values, this function
    returns either 0 or 1, and never -1.

    \param x  The function's argument.

    \retval 0  \p x represents zero.
    \retval 1  \p x represents a positive value.

    \see  boost::math::big_radix_whole::sign_self

    \relates  boost::math::big_radix_whole
 */
template < int Radix, class Allocator >
inline
int
boost::math::sgn
(
    boost::math::big_radix_whole<Radix, Allocator> const &  x
)
{
    return x ? +1 : 0;
}

/** \brief  Digit-sum function for \c big_radix_whole\<\>

    Computes the sum of all the digits represented within the given
    \c big_radix_whole\<\> object's value.  Note that single-digit values,
    including zero, will return themselves. 

    \param x  The function's argument.

    \return  <code><b>Sum</b>{<var>i</var>}( <var>x</var>.digit_at(<var>i</var>)
             )</code>

    \see  boost::math::big_radix_whole::digit_summate_self

    \relates  boost::math::big_radix_whole
 */
template < int Radix, class Allocator >
inline
boost::math::big_radix_whole<Radix, Allocator>
boost::math::digit_sum
(
    boost::math::big_radix_whole<Radix, Allocator> const &  x
)
{
    big_radix_whole<Radix, Allocator>  result( x );

    result.digit_summate_self();
    return result;
}


#endif  // BOOST_MATH_BIG_RADIX_WHOLE_IMPL_MATH_IPP

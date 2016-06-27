//  Boost math/impl/big_radix_whole_shifting.ipp header file  ----------------//

//  Copyright 2006 Daryle Walker.  Distributed under the Boost Software License,
//  Version 1.0.  (See the accompanying file LICENSE_1_0.txt or a copy at
//  <http://www.boost.org/LICENSE_1_0.txt>.)

//  See <http://www.boost.org/libs/math/> for the library's home page.

/** \file
    \brief  Part of the implementation for an arbitrary-length specific-radix
            unsigned integer class template

    Contains the definitions some of the public member functions of the
    \c boost::math::big_radix_whole\<\> class template.  All of these member
    functions, including some operators, perform various place-shiftings.
 */

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_IMPL_SHIFTING_IPP
#define BOOST_MATH_BIG_RADIX_WHOLE_IMPL_SHIFTING_IPP

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_CORE_HPP
#error "#Include only as part of <boost/math/big_radix_whole_core.hpp>"
#endif

#include <boost/assert.hpp>  // for BOOST_ASSERT


//  Radix/bignum/natural extra member function definitions  ------------------//

/** Appends a digit to the current number in the one's place.  It should be
    faster than using a left-shift (or multiply) followed by an addition.

    \pre  <code>0 &lt;= <var>value</var> &lt; Radix</code>

    \param value  The new one's digit

    \post  <code>*this == <var>value</var> + Radix * <var>old_this</var></code>
 */
template < int Radix, class Allocator >
void
boost::math::big_radix_whole<Radix, Allocator>::shift_up_add_single
(
    digit_type  value
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    BOOST_ASSERT( (value >= 0) && (value < self_type::radix) );

    if ( !this->digits_.empty() || value )
    {
        this->digits_.push_front( value );
    }
    // ELSE: don't insert a zero into an empty digit-string (it's already zero)

    BOOST_ASSERT( this->test_invariant() );
}

/** Removes the one's digit from the current number, moving the higher digits
    down by one place.  The removed digit value is returned.  This should be
    faster than a modulo followed by a right-shift (or divide).

    \return  The old one's digit

    \post  <code><var>old_this</var> == <var>return_value</var> + *this *
           Radix</code>
 */
template < int Radix, class Allocator >
typename boost::math::big_radix_whole<Radix, Allocator>::digit_type
boost::math::big_radix_whole<Radix, Allocator>::mod_radix_shift_down
(
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    digit_type  result;

    if ( !this->digits_.empty() )
    {
        result = this->digits_.front();
        this->digits_.pop_front();
    }
    else
    {
        result = 0;
    }

    BOOST_ASSERT( this->test_invariant() );
    return result;
}


//  Radix/bignum/natural operator member function definitions  ---------------//

/** Amplifies the current value by a given amount of magnitude, filling the
    lower places with zeros.

    \param amount  How many digits to shift up.  (Don't make it too large, to
                   avoid memory problems.)

    \return  A reference to <code>*this</code> object.

    \post  <code>*this == <var>old_this</var> *
           Radix<sup><var>amount</var></sup></code>

    \note  The class declaration uses the Boost.Operators library to synthesize
           the left-shift <code>operator &lt;&lt;</code> from this one.

    \see  #shift_up_add_single
 */
template < int Radix, class Allocator >
boost::math::big_radix_whole<Radix, Allocator> &
boost::math::big_radix_whole<Radix, Allocator>::operator <<=(
    size_type  amount
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    if ( amount && !this->digits_.empty() )
    {
        this->digits_.insert( this->digits_.begin(), amount, 0 );
    }
    // ELSE: shifting of and/or by zero doesn't change the current value

    BOOST_ASSERT( this->test_invariant() );
    return *this;
}

/** Attenuates the current value by a given amount of magnitude, truncating the
    places that became sub-integer.

    \param amount  How many digits to shift down.  If it is greater than or
                   equal to <code>this-&gt;digit_count()</code>, then the
                   current object's value reduces to zero.

    \return  A reference to <code>*this</code> object.

    \post  <code>*this == <var>old_this</var> <b>div</b>
           Radix<sup><var>amount</var></sup></code>

    \note  The class declaration uses the Boost.Operators library to synthesize
           the right-shift <code>operator &gt;&gt;</code> from this one.

    \see  #mod_radix_shift_down
 */
template < int Radix, class Allocator >
boost::math::big_radix_whole<Radix, Allocator> &
boost::math::big_radix_whole<Radix, Allocator>::operator >>=(
    size_type  amount
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    if ( amount && !this->digits_.empty() )
    {
        if ( this->digits_.size() > amount )
        {
            iterator const  b = this->digits_.begin();

            this->digits_.erase( b, b + amount );
        }
        else
        {
            this->digits_.clear();
        }
    }
    // ELSE: shifting of and/or by zero doesn't change the current value

    BOOST_ASSERT( this->test_invariant() );
    return *this;
}


#endif  // BOOST_MATH_BIG_RADIX_WHOLE_IMPL_SHIFTING_IPP

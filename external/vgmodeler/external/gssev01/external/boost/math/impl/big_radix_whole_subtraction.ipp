//  Boost math/impl/big_radix_whole_subtraction.ipp header file  -------------//

//  Copyright 2006 Daryle Walker.  Distributed under the Boost Software License,
//  Version 1.0.  (See the accompanying file LICENSE_1_0.txt or a copy at
//  <http://www.boost.org/LICENSE_1_0.txt>.)

//  See <http://www.boost.org/libs/math/> for the library's home page.

/** \file
    \brief  Part of the implementation for an arbitrary-length specific-radix
            unsigned integer class template

    Contains the definitions some of the public member functions of the
    \c boost::math::big_radix_whole\<\> class template, and some related
    non-member functions.  All of these (member) functions, including some
    operators, perform various subtractions.
 */

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_IMPL_SUBTRACTION_IPP
#define BOOST_MATH_BIG_RADIX_WHOLE_IMPL_SUBTRACTION_IPP

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_CORE_HPP
#error "#Include only as part of <boost/math/big_radix_whole_core.hpp>"
#endif

#include <algorithm>  // for std::lexicographical_compare, transform

#include <boost/assert.hpp>         // for BOOST_ASSERT
#include <boost/lambda/lambda.hpp>  // for boost::lamda::_1


//  Radix/bignum/natural extra member function definitions  ------------------//

/** Replaces the current value with its additive inverse.

    \pre  The negative of the represented value is also representable.  Since
          this type is limited to representing nonnegative values, only an
          object with a zero value is permissible.

    \throws boost::math::big_radix_whole_negative_result_error
             The current state describes a positive value, which would have
             necessitated a negative value for the new state, which is not
             representable.

    \post  <code>0 == *this + <var>old_this</var></code>

    \note  Since the state of the object either doesn't change or throws an
           exception, no mutable action is done.  So this member function can be
           <code>const</code>.
 */
template < int Radix, class Allocator >
inline
void
boost::math::big_radix_whole<Radix, Allocator>::negate_self
(
) const
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    if ( !this->digits_.empty() )
    {
        throw big_radix_whole_negative_result_error(
         "attempted to negate positive value" );
    }
    // ELSE: zero is transformed to zero -> no need to change object's state
}

/** Decreases a digit of the current number in the given place, propagating
    borrows as needed.  It should be faster than using a left-shift (or
    multiply) followed by a subtraction.

    \pre  <code>0 &lt;= <var>value</var> &lt; Radix</code>
    \pre  <code><var>value</var> * Radix<sup><var>index</var></sup> &lt;=
          *this</code>

    \param value  The subtrahend to be subtracted from the specified digit
    \param index  The place of the specified digit

    \throws boost::math::big_radix_whole_negative_result_error
             The current state describes a value less than the subtrahend, which
             would require a negative value for the new state, which is not
             representable.

    \post  <code>*this == <var>old_this</var> - <var>value</var> *
           Radix<sup><var>index</var></sup></code>
 */
template < int Radix, class Allocator >
void
boost::math::big_radix_whole<Radix, Allocator>::subtract_shifted_single
(
    digit_type  value,
    size_type   index
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    BOOST_ASSERT( 0 <= value && value < self_type::radix );

    if ( value )
    {
        size_type const  s = this->digits_.size();

        if ( index >= s || index == s - 1u && value > this->digits_[index] )
        {
            // (self_type(value) << index) > *this
            throw big_radix_whole_negative_result_error( "attempted to subtract"
             " a larger single-precision number (possibly after shifting)" );
        }
        else
        {
            // (self_type(value) << index) <= *this
            iterator  i = this->digits_.begin() + index;

            *i -= value;

            // No borrows can cascade beyond the highest-placed digit, so don't
            // bother with a "this->digits_.end()" check.
            while ( 0 > *i )
            {
                *i += self_type::radix;
                --*++i;
            }

            // Keep invariants in "1..." -> "0..." cases
            this->clear_leading_zeros();
        }
    }
    // ELSE: something - 0 == that something -> no change

    BOOST_ASSERT( this->test_invariant() );
}

/** Decreases the value of the current number by a (non-negative) value less
    than the radix.  It should be faster than converting the short subtrahend to
    a full \c big_radix_whole\<\> before subtracting.

    \pre  <code>0 &lt;= <var>subtrahend</var> &lt; Radix</code>
    \pre  <code><var>subtrahend</var> &lt;= *this</code>

    \param subtrahend  The value to be subtracted

    \throws boost::math::big_radix_whole_negative_result_error
             The current state describes a value less than the
             <var>subtrahend</var>, which would require a negative value for the
             new state, which is not representable.

    \post  <code>*this == <var>old_this</var> - <var>subtrahend</var></code>

    \see  #subtract_shifted_single
 */
template < int Radix, class Allocator >
inline
void
boost::math::big_radix_whole<Radix, Allocator>::subtract_single
(
    digit_type  subtrahend
)
{
    this->subtract_shifted_single( subtrahend, 0u );
}

/** Replaces the current value with its absolute value after decreasing one of
    its digits, at a given place, by a given value (that's non-negative and less
    than the radix).  It should be faster than using a left-shift, or multiply,
    followed by a subtraction (and don't forget the sign and magnitude checks).

    \pre  <code>0 &lt;= <var>value</var> &lt; Radix</code>
    \pre  <code><var>index</var> &lt; this-&gt;digit_limit()</code>

    \param value  The subtrahend to be subtracted from the specified digit
    \param index  The place of the specified digit

    \retval true   The difference was originally negative (i.e.
                   <code><var>value</var> * Radix<sup><var>index</var></sup>
                   &gt; *this</code>).
    \retval false  The difference was originally non-negative.

    \post  <code>*this == |<var>old_this</var> - <var>value</var> *
           Radix<sup><var>index</var></sup>|</code>
 */
template < int Radix, class Allocator >
bool
boost::math::big_radix_whole<Radix,
 Allocator>::subtract_shifted_single_absolutely
(
    digit_type  value,
    size_type   index
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    BOOST_ASSERT( 0 <= value && value < self_type::radix );
    BOOST_ASSERT( index < this->digits_.max_size() );

    bool  result = false;

    if ( value )
    {
        size_type  s = this->digits_.size();

        if ( index < s )
        {
            // Just subtract the digit...
            iterator const  d = this->digits_.begin() + index;

            *d -= value;

            // ...and propagate the borrow
            iterator const  last_i = this->digits_.end() - 1;

            for ( iterator  i = d ; (last_i > i) && (0 > *i) ; )
            {
                // The negative-value test is already in the FOR loop
                *i += self_type::radix;
                --*++i;
            }

            // Compensate for "1..." to "0..." cases
            this->clear_leading_zeros();
        }
        else
        {
            // Append the space for the digit and intermediate zeros
            // (in one shot to reduce the number of throw spots)
            this->digits_.insert( this->digits_.end(), index - s + 1u, 0 );

            // Add (actually subtract) the digit
            this->digits_.back() = -value;
        }

        // A larger subtrahend gives a negative value in the highest place.
        s = this->digits_.size();
        if ( result = (s && ( 0 > this->digits_.back() )) )
        {
            // Get the absolute value by negating all the digits...
            iterator const  last_j = this->digits_.end() - 1;
            bool            borrow = false;

            for ( iterator  j = this->digits_.begin() ; last_j > j ; ++j )
            {
                reference  digit = *j;

                // ...and then normalize
                digit += static_cast<int>( borrow );
                if ( borrow = static_cast<bool>(digit) )
                {
                    digit = self_type::radix - digit;
                }
            }

            // Propagate the remaining borrow when negating highest digit
            reference  last_d = *last_j;

            last_d = -last_d - static_cast<int>( borrow );
            this->clear_leading_zeros();
        }
    }
    // ELSE: something - 0 == that something -> no change

    BOOST_ASSERT( this->test_invariant() );
    return result;
}

/** Replaces the current value with the absolute difference between that value
    and another (non-negative) value that is less than the radix.  It should be
    faster than converting the short subtrahend to a full \c big_radix_whole\<\>
    before subtracting.

    \pre  <code>0 &lt;= <var>subtrahend</var> &lt; Radix</code>

    \param subtrahend  The value to be subtracted

    \retval true   The difference was originally negative (i.e.
                   <code><var>subtrahend</var> &gt; *this</code>).
    \retval false  The difference was originally non-negative.

    \post  <code>*this == |<var>old_this</var> - <var>subtrahend</var>|</code>

    \see  #subtract_shifted_single_absolutely
 */
template < int Radix, class Allocator >
inline
bool
boost::math::big_radix_whole<Radix, Allocator>::subtract_single_absolutely
(
    digit_type  subtrahend
)
{
    return this->subtract_shifted_single_absolutely( subtrahend, 0u );
}

/** Decreases the digit-values of the current number starting at the given place
    by a given value, propagating borrows as needed.  It should be faster than
    using a left-shift (or multiply) followed by a subtraction.

    \pre  <code><var>value</var> * Radix<sup><var>index</var></sup> &lt;=
          *this</code>

    \param value  The subtrahend to be subtracted starting at the specified
                  digit
    \param index  The place of the specified digit

    \throws boost::math::big_radix_whole_negative_result_error
             The current state describes a value less than the subtrahend, which
             would require a negative value for the new state, which is not
             representable.

    \post  <code>*this == <var>old_this</var> - <var>value</var> *
           Radix<sup><var>index</var></sup></code>
 */
template < int Radix, class Allocator >
void
boost::math::big_radix_whole<Radix, Allocator>::subtract_shifted_full
(
    big_radix_whole const &  value,
    size_type                index
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );
    BOOST_PRIVATE_WILD_ASSERT( value.test_invariant() );

    deque_type const &  vd = value.digits_;
    deque_type &        td = this->digits_;

    if ( !vd.empty() )
    {
        size_type const  ts = td.size(), vs = vd.size();

        if ( (vs > ts) || (index > ts - vs) || (( index == ts - vs ) &&
         std::lexicographical_compare( td.rbegin(), td.rbegin() + vs,
         vd.rbegin(), vd.rend() )) )
        {
            throw big_radix_whole_negative_result_error( "attempted to subtract"
             " a larger full-precision number (possibly after shifting)" );
        }
        else
        {
            using namespace boost::lambda;

            // Subtrahend will fit; do the subtraction
            iterator const  tb = td.begin() + index;
            iterator        ti = tb;

            ti = std::transform( vd.begin(), vd.end(), tb, ti, _2 - _1 );

            // Resolve internal borrows
            for ( iterator  i = tb ; i < ti ; ++i )
            {
                reference  digit = *i;

                if ( 0 > digit )   // must be skipped when i == td.end() - 1
                {
                    digit += self_type::radix;
                    --*(i + 1);
                }
            }

            // Resolve propagating borrows
            iterator const  te = td.end();

            while ( (te > ti) && (0 > *ti) )
            {
                *ti++ += self_type::radix;
                --*ti;
            }

            // Sometimes the peak digit is wiped out
            this->clear_leading_zeros();
        }
    }
    // ELSE: anything - 0 == anything -> no change

    BOOST_ASSERT( this->test_invariant() );
}

/** Replaces the current number with its absolute value after decreasing the
    digit-values starting at the given place by a given value.  It should be
    faster than using a left-shift (or multiply) followed by a subtraction (and
    sign and magnitude checks).

    \pre  <code><var>index</var> + <var>value</var>.digit_count() &lt;=
          this-&gt;digit_limit()</code>

    \param value  The subtrahend to be subtracted starting at the specified
                  digit
    \param index  The place of the specified digit

    \retval true   The difference was originally negative (i.e.
                   <code><var>value</var> * Radix<sup><var>index</var></sup>
                   &gt; *this</code>).
    \retval false  The difference was originally non-negative.

    \post  <code>*this == |<var>old_this</var> - <var>value</var> *
           Radix<sup><var>index</var></sup>|</code>
 */
template < int Radix, class Allocator >
bool
boost::math::big_radix_whole<Radix, Allocator>::subtract_shifted_full_absolutely
(
    big_radix_whole const &  value,
    size_type                index
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );
    BOOST_PRIVATE_WILD_ASSERT( value.test_invariant() );

    deque_type const &  vd = value.digits_;
    deque_type &        td = this->digits_;

    // Structure assert to avoid under/over-flow
    BOOST_ASSERT( (vd.size() <= td.max_size()) && (index <= td.max_size() -
     vd.size()) );

    bool  result = false;

    if ( !vd.empty() )
    {
        using namespace boost::lambda;

        using std::transform;

        size_type const  ts = td.size(), vs = vd.size(), ss = index + vs;

        if ( ss > ts )
        {
            // Zero-extend the minuend to fit the subtrahend
            td.insert( td.end(), ss - ts, 0 );
        }

        // Do the subtraction
        iterator const  tb = td.begin(), tl = tb + index;
        iterator        ti = tl;

        ti = transform( vd.begin(), vd.end(), tl, ti, _2 - _1 );

        // Resolve internal carries
        for ( iterator  i = tl + 1 ; ti > i ; ++i )
        {
            reference  previous = *(i - 1), current = *i;

            while ( 0 > previous )
            {
                previous += self_type::radix;
                --current;
            }
        }

        // Resolve propagating carries
        iterator const  te = td.end();

        for ( iterator  j = ti ; (te > j) && (0 > *( j - 1 )) ; ++j )
        {
            reference  previous = *(j - 1), current = *j;

            while ( 0 > previous )
            {
                previous += self_type::radix;
                --current;
            }
        }
        this->clear_leading_zeros();

        // Negate the result if necessary
        if ( (result = (!td.empty() && ( 0 > td.back() )) ))
        {
            // (The previous clear_leading_zeros may have invalidated tb & te.)
            iterator const  tb2 = td.begin(), te2 = td.end(), tp = te2 - 1;

            transform( tb2, te2, tb2, -_1 );
            for ( iterator  k = tb2 ; tp > k ; )
            {
                reference  current = *k, next = *++k;

                while ( 0 > current )
                {
                    current += self_type::radix;
                    --next;
                }
            }
            BOOST_ASSERT( *tp >= 0 );

            this->clear_leading_zeros();
        }
    }
    // ELSE: |anything - 0| == |anything| -> no change for any non-negative

    BOOST_ASSERT( this->test_invariant() );
    return result;
}

/** Replaces the current number with the absolute difference between its value
    and another.

    \param subtrahend  The value to be subtracted

    \retval true   The difference was originally negative (i.e.
                   <code><var>subtrahend</var> &gt; *this</code>).
    \retval false  The difference was originally non-negative.

    \post  <code>*this == |<var>old_this</var> - <var>subtrahend</var>|</code>

    \see  #subtract_shifted_full_absolutely
 */
template < int Radix, class Allocator >
inline
bool
boost::math::big_radix_whole<Radix, Allocator>::subtract_full_absolutely
(
    big_radix_whole const &  subtrahend
)
{
    return this->subtract_shifted_full_absolutely( subtrahend, 0u );
}


//  Radix/bignum/natural operator member function definitions  ---------------//

/** Replaces the current value with the adjacent integer value towards negative
    infinity.

    \pre  The current represented value is positive (i.e. not zero).

    \return  A reference to <code>*this</code> object.

    \throws boost::math::big_radix_whole_negative_result_error
             The current state describes a zero value, which would have
             necessitated a negative value for the new state, which is not
             representable.

    \post  <code>1 == <var>old_this</var> - *this</code>

    \note  The class declaration uses the Boost.Operators library to synthesize
           the post-decrement <code>operator --</code> from this one.
 */
template < int Radix, class Allocator >
boost::math::big_radix_whole<Radix, Allocator> &
boost::math::big_radix_whole<Radix, Allocator>::operator --
(
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    if ( !this->digits_.empty() )
    {
        digit_type const  highest_digit = self_type::radix - 1;
        bool              borrow = true;
        iterator const    e = this->digits_.end();

        // Due to the invariant, this loop must stop before the end
        for ( iterator  i = this->digits_.begin() ; borrow && (e != i) ; ++i )
        {
            reference  digit = *i;

            if ( borrow = !digit )  // DELIBERATE ASSIGN-THEN-TEST
            {
                digit = highest_digit;
            }
            else
            {
                --digit;
            }
        }

        // Compensate going from "radix ** (digit_count - 1)" to "digit_count
        // - 1" copies of "radix - 1" digits.
        if ( !this->digits_.back() )
        {
            this->digits_.pop_back();
        }
    }
    else
    {
        // Negative values are not supported, so the
        // 0 -> -1 transition can't happen.
        throw big_radix_whole_negative_result_error( "decremented below zero" );
    }

    BOOST_ASSERT( this->test_invariant() );
    return *this;
}

/** Decreases the current number by a given value.

    \pre  <code><var>subtrahend</var> &lt;= *this</code>

    \param subtrahend  The value to be subtracted from the current number, which
                       serves as the minuend.

    \return  A reference to <code>*this</code> object as the difference.

    \throws boost::math::big_radix_whole_negative_result_error
             The current state describes a value less than the subtrahend, which
             would require a negative value for the new state, which is not
             representable.

    \post  <code>*this == <var>old_this</var> <b>-</b>
           <var>subtrahend</var></code>

    \note  The class declaration uses the Boost.Operators library to synthesize
           the (binary) subtraction <code>operator -</code> from this one.

    \see  #subtract_shifted_full
 */
template < int Radix, class Allocator >
inline
boost::math::big_radix_whole<Radix, Allocator> &
boost::math::big_radix_whole<Radix, Allocator>::operator -=
(
    big_radix_whole const &  subtrahend
)
{
    this->subtract_shifted_full( subtrahend, 0u );
    return *this;
}


//  Radix/bignum/natural non-member operator function definitions  -----------//

/** \brief  Unary-minus operator for \c big_radix_whole\<\>

    Returns the additive inverse of the operand.

    \param x  The operand to be negated.

    \return  A value <var>y</var> such that <code><var>x</var> + <var>y</var>
             == 0</code>.

    \throws boost::math::big_radix_whole_negative_result_error
             Only if \p x is positive, since negative values are forbidden.

    \see  boost::math::big_radix_whole::negate_self

    \relates  boost::math::big_radix_whole
 */
template < int Radix, class Allocator >
inline
boost::math::big_radix_whole<Radix, Allocator>
boost::math::operator -
(
    big_radix_whole<Radix, Allocator> const &  x
)
{
    return x ? throw big_radix_whole_negative_result_error(
     "negative values banned" ) : x;
}


#endif  // BOOST_MATH_BIG_RADIX_WHOLE_IMPL_SUBTRACTION_IPP

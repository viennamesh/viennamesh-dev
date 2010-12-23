//  Boost math/impl/big_radix_whole_addition.ipp header file  ----------------//

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
    operators, perform various additions.
 */

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_IMPL_ADDITION_IPP
#define BOOST_MATH_BIG_RADIX_WHOLE_IMPL_ADDITION_IPP

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_CORE_HPP
#error "#Include only as part of <boost/math/big_radix_whole_core.hpp>"
#endif

#include <algorithm>  // for std::find_if, fill_n, max, copy, transform

#include <boost/assert.hpp>         // for BOOST_ASSERT
#include <boost/lambda/lambda.hpp>  // for boost::lamda::_1


//  Radix/bignum/natural extra member function definitions  ------------------//

/** Replaces the current value with itself.

    \post  <code>*this == <var>old_this</var></code>

    \note  Since the state of the object doesn't change, no actual action is
           done.  So this member function can be <code>const</code>.
 */
template < int Radix, class Allocator >
inline
void
boost::math::big_radix_whole<Radix, Allocator>::identify_self
(
) const
{
    // Since nothing happens, don't bother with consistency checks
}

/** Increases a digit of the current number in the given place, propagating
    carries as needed.  It should be faster than using a left-shift (or
    multiply) followed by an addition.

    \pre  <code>0 &lt;= <var>value</var> &lt; Radix</code>
    \pre  <code><var>index</var> + 1 &lt; this-&gt;digit_limit()</code>

    \param value  The addend to be added to the specified digit
    \param index  The place of the specified digit

    \post  <code>*this == <var>old_this</var> + <var>value</var> *
           Radix<sup><var>index</var></sup></code>
 */
template < int Radix, class Allocator >
void
boost::math::big_radix_whole<Radix, Allocator>::add_shifted_single
(
    digit_type  value,
    size_type   index
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    BOOST_ASSERT( 0 <= value && value < self_type::radix );
    BOOST_ASSERT( index < this->digits_.max_size() - 1u );

    size_type const  s = this->digits_.size();

    if ( index < s )
    {
        reference  digit = this->digits_[ index ];

        if ( self_type::radix - digit > value )
        {
            // No carry -> add directly
            digit += value;
        }
        else
        {
            // Check for cascading carries
            iterator const  b = this->digits_.begin();

            size_type const  start = index + 1u;
            size_type const  stop = std::find_if( b + start, this->digits_.end()
             , lambda::_1 != self_type::radix - 1 ) - b;

            // Take care of the carry (done first since it may allocate)
            if ( stop == s )
            {
                this->digits_.push_back( 1 );
            }
            else
            {
                ++this->digits_[ stop ];
            }

            // Turn over any intermediate "radix - 1" digits
            // (Can't use "b" since the "push_back" would invalidate it.)
            std::fill_n( this->digits_.begin() + start, stop - start, 0 );

            // Add, but remove the already compensated-for carry
            digit += value;
            digit -= self_type::radix;
        }
    }
    else if ( value )
    {
        // The new value is outside the current set of digits, so it's
        // basically appended to the number, along with some intermediate
        // zero-valued digits.
        this->digits_.insert( this->digits_.end(), index - s + 1u, 0 );

        // The spot for the new digit is allocated with the intermediate
        // zero-value digits so only one allocation call is used,
        // eliminating a potential throw spot.
        this->digits_.back() = value;
    }
    // ELSE: don't uselessly append zero-valued digit(s)

    BOOST_ASSERT( this->test_invariant() );
}

/** Increases the value of the current number by a (non-negative) value less
    than the radix.  It should be faster than converting the short addend to a
    full \c big_radix_whole\<\> before adding.

    \pre  <code>0 &lt;= <var>addend</var> &lt; Radix</code>

    \param addend  The value to be added

    \post  <code>*this == <var>old_this</var> + <var>addend</var></code>

    \see  #add_shifted_single
 */
template < int Radix, class Allocator >
inline
void
boost::math::big_radix_whole<Radix, Allocator>::add_single
(
    digit_type  addend
)
{
    this->add_shifted_single( addend, 0u );
}

/** Increases the digit-values of the current number starting at the given place
    by a given value, propagating carries as needed.  It should be faster than
    using a left-shift (or multiply) followed by an addition.

    \pre  <code><b>Max</b>( this-&gt;digit_count(), <var>index</var> +
          <var>value</var>.digit_count() ) + 1 &lt;=
          this-&gt;digit_limit()</code>

    \param value  The addend to be added starting at the specified digit
    \param index  The place of the specified digit

    \post  <code>*this == <var>old_this</var> + <var>value</var> *
           Radix<sup><var>index</var></sup></code>
 */
template < int Radix, class Allocator >
void
boost::math::big_radix_whole<Radix, Allocator>::add_shifted_full
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
     vd.size()) && (std::max( td.size(), vd.size() + index ) <= td.max_size() -
     1u) );

    if ( !vd.empty() )
    {
        using std::copy;

        size_type const  ts = td.size(), vs = vd.size(), as = index + vs;

        if ( index > ts )
        {
            // Append the shifted addend digits directly, with intermediate
            // zeros; there's no actual addition involved
            td.insert( td.end(), as - ts, 0 );
            copy( vd.begin(), vd.end(), td.begin() + index );
        }
        else if ( index == ts )
        {
            // Like the previous case, but take a shortcut since there are no
            // intermediate zeros
            td.insert( td.end(), vd.begin(), vd.end() );
        }
        else  // index < ts
        {
            using namespace boost::lambda;

            // Check if addend runs past augend
            const_iterator  v_break;

            if ( as > ts )
            {
                // Allocate space for addend digits that go beyond the augend's
                // size, plus a spare for a cascading carry
                td.insert( td.end(), as - ts + 1u, 0 );
                v_break = vd.begin() + ( ts - index );
                copy( v_break, vd.end(), td.begin() + ts );
            }
            else
            {
                // Allocate space for cascading carry
                td.push_back( 0 );
                v_break = vd.end();
            }

            // Add the digits together
            iterator const  tb = td.begin() + index;
            iterator        ti = tb;

            ti = std::transform( vd.begin(), v_break, tb, ti, _2 + _1 );

            // Resolve internal carries
            for ( iterator  i = tb ; i < ti ; ++i )
            {
                reference  digit = *i;

                if ( digit >= self_type::radix )
                {
                    digit -= self_type::radix;
                    ++*(i + 1);
                }
            }

            // Resolve propagating carries
            while ( *ti >= self_type::radix )
            {
                *ti -= self_type::radix;
                ++*++ti;
            }
            BOOST_ASSERT( ti < td.end() );

            // Sometimes a spare carry digit isn't used
            this->clear_leading_zeros();
        }
    }
    // ELSE: anything + 0 == anything -> no change

    BOOST_ASSERT( this->test_invariant() );
}


//  Radix/bignum/natural operator member function definitions  ---------------//

/** Replaces the current value with the adjacent integer value towards infinity.

    \return  A reference to <code>*this</code> object.

    \post  <code>1 == *this - <var>old_this</var></code>

    \note  The class declaration uses the Boost.Operators library to synthesize
           the post-increment <code>operator ++</code> from this one.
 */
template < int Radix, class Allocator >
boost::math::big_radix_whole<Radix, Allocator> &
boost::math::big_radix_whole<Radix, Allocator>::operator ++
(
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    // If the one's place digit isn't "radix - 1", then we can just do an
    // increment.  Otherwise, all those lowest-place "radix - 1" values get
    // turned into zeros through a cascade of carries.  The lowest
    // non-"radix - 1" place then goes up by one.  (An offset is stored, instead
    // of an iterator, because the "else" block could invalidate iterators.)
    difference_type const  offset = std::find_if( this->digits_.begin(),
     this->digits_.end(), lambda::_1 != self_type::radix - 1 ) -
     this->digits_.begin();

    if ( static_cast<size_type>(offset) < this->digits_.size() )
    {
        // Simply increment the digit.  This case applies even if there wasn't a
        // block of "radix - 1" values at the lowest places.
        ++this->digits_[ offset ];
    }
    else
    {
        // All digits were "radix - 1", so new value is "radix ** digit_count".
        // This case applies even for empty digit-lists, i.e. zero.
        this->digits_.push_back( 1 );
    }

    // Either way, the digits below the incremented/appended one turn over.
    std::fill_n( this->digits_.begin(), offset, 0 );

    BOOST_ASSERT( this->test_invariant() );
    return *this;
}

/** Increases the current number by a given value.

    \param addend  The value to be added to the current number, which serves as
                   the augend.  (Don't make it too large, to avoid memory
                   problems.)

    \return  A reference to <code>*this</code> object as the sum.

    \post  <code>*this == <var>old_this</var> <b>+</b> <var>addend</var></code>

    \note  The class declaration uses the Boost.Operators library to synthesize
           the (binary) addition <code>operator +</code> from this one.

    \see  #add_shifted_full
 */
template < int Radix, class Allocator >
inline
boost::math::big_radix_whole<Radix, Allocator> &
boost::math::big_radix_whole<Radix, Allocator>::operator +=
(
    big_radix_whole const &  addend
)
{
    this->add_shifted_full( addend, 0u );
    return *this;
}


//  Radix/bignum/natural non-member operator function definitions  -----------//

/** \brief  Unary-plus operator for \c big_radix_whole\<\>

    Returns a copy of the operand.  Included for completeness.

    \param x  The operand to be confirmed.

    \return  \p x

    \see  boost::math::big_radix_whole::identify_self

    \relates  boost::math::big_radix_whole
 */
template < int Radix, class Allocator >
inline
boost::math::big_radix_whole<Radix, Allocator>
boost::math::operator +
(
    big_radix_whole<Radix, Allocator> const &  x
)
{
    return x;
}


#endif  // BOOST_MATH_BIG_RADIX_WHOLE_IMPL_ADDITION_IPP

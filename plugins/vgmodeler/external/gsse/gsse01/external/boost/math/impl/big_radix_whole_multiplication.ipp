//  Boost math/impl/big_radix_whole_multiplication.ipp header file  ----------//

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
    operators, perform various multiplications.
 */

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_IMPL_MULTIPLICATION_IPP
#define BOOST_MATH_BIG_RADIX_WHOLE_IMPL_MULTIPLICATION_IPP

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_CORE_HPP
#error "#Include only as part of <boost/math/big_radix_whole_core.hpp>"
#endif

#include <algorithm>  // for std::transform
#include <cstdlib>    // for std::div, div_t

#include <boost/assert.hpp>         // for BOOST_ASSERT
#include <boost/lambda/lambda.hpp>  // for boost::lamda::_1, _2


//  Radix/bignum/natural extra member function definitions  ------------------//

/** Amplifies the current number by a given (non-negative) value, then increases
    the result by another given (non-negative) value, with both values less than
    the radix.  It should be faster than converting either the short multiplier
    or the short addend to full \c big_radix_whole\<\> objects before
    multiplying and adding.

    \pre  <code>0 &lt;= <var>augend_multiplier</var>, <var>addend</var> &lt;
          Radix</code>

    \param augend_multiplier  The multipiler to the current object in the first
                              step.  The current object becomes the product
                              serving as the augend.
    \param addend             The addend to the current object in the second
                              step.

    \post  <code>*this == <var>old_this</var> * <var>augend_multiplier</var> +
           <var>addend</var></code>
 */
template < int Radix, class Allocator >
void
boost::math::big_radix_whole<Radix, Allocator>::multiply_single_add_single
(
    digit_type  augend_multiplier,
    digit_type  addend
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    BOOST_ASSERT( 0 <= augend_multiplier && augend_multiplier < self_type::radix
     && 0 <= addend && addend < self_type::radix );

    if ( size_type const  s = this->digits_.size() )
    {
        // Append scratch space for the carry
        this->digits_.push_back( 0 );

        // Perform one loop of multiplication
        std::div_t  qr;

        qr.quot = addend;

        for ( size_type  i = 0u ; s >= i ; ++i )
        {
            reference  digit = this->digits_[ i ];

            qr = std::div( digit * augend_multiplier + qr.quot,
             self_type::radix );
            digit = qr.rem;
        }
        BOOST_ASSERT( 0 == qr.quot );  // scratch place doesn't make carry

        // Handle scratch space, or if augend-multiplier was zero
        this->clear_leading_zeros();
    }
    else if ( addend )
    {
        // 0 * anything1 + anything2 == anything2
        this->digits_.push_back( addend );
    }
    // ELSE: 0 * anything + 0 == 0 -> no change

    BOOST_ASSERT( this->test_invariant() );
}

/** Amplifies the current number by a given (non-negative) value, then decreases
    the result by another given (non-negative) value, with both values less than
    the radix.  It should be faster than converting either the short multiplier
    or the short subtrahend to full \c big_radix_whole\<\> objects before
    multiplying and subtracting.

    \pre  <code>0 &lt;= <var>minuend_multiplier</var>, <var>subtrahend</var>
          &lt; Radix</code>

    \param minuend_multiplier  The multipiler to the current object in the first
                               step.  The current object becomes the product
                               serving as the minuend.
    \param subtrahend          The subtrahend to the current object in the
                               second step.

    \throws boost::math::big_radix_whole_negative_result_error
             The current state describes a value that after multipling it by the
             minuend-multiplier is still less than the subtrahend, which would
             require a negative value for the new state, which is not
             representable.

    \post  <code>*this == <var>old_this</var> * <var>minuend_multiplier</var> -
           <var>subtrahend</var></code>
 */
template < int Radix, class Allocator >
void
boost::math::big_radix_whole<Radix, Allocator>::multiply_single_subtract_single
(
    digit_type  minuend_multiplier,
    digit_type  subtrahend
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    BOOST_ASSERT( 0 <= minuend_multiplier && minuend_multiplier <
     self_type::radix && 0 <= subtrahend && subtrahend < self_type::radix );

    big_radix_whole_negative_result_error const  exception( "attempted to "
     "subtract from a smaller short-product" );

    if ( minuend_multiplier )
    {
        using std::div;

        std::div_t  qr;
        int &       scratch = qr.quot;  // just a memory-saving shortcut

        switch ( size_type const  s = this->digits_.size() )
        {
        default:
            // Since the object's value is at least Radix, the single-digit
            // minuend-multiplier is at least 1, and the subtrahend is at most
            // Radix - 1, the operation can't go negative.

            // Append scratch space for the multiplication carry
            this->digits_.push_back( 0 );

            // Perform one loop of multiplication
            qr.quot = -subtrahend;

            for ( size_type  i = 0u ; s >= i ; ++i )
            {
                reference  digit = this->digits_[ i ];

                qr = div( digit * minuend_multiplier + qr.quot,
                 self_type::radix );
                digit = qr.rem;
            }
            BOOST_ASSERT( 0 == qr.quot );  // scratch place doesn't make carry

            // Make sure borrows are resolved (due to Standard not specifying
            // how division with negatives distributes the quot/rem signs)
            for ( size_type  j = 0u ; s > j ; ++j )
            {
                reference  digit = this->digits_[ j ];

                while ( 0 > digit )
                {
                    digit += self_type::radix;
                    --this->digits_[ j + 1u ];
                }
            }
            BOOST_ASSERT( 0 <= this->digits_.back() );  // no negative results

            break;

        case 1u:
            if ( (scratch = this->digits_.front() * minuend_multiplier -
             subtrahend) < 0 )
            {
                throw exception;
            }
            else
            {
                if ( (qr = div( scratch, self_type::radix )).quot )
                {
                    this->digits_.push_back( qr.quot );
                }
                this->digits_.front() = qr.rem;
            }
            break;

        case 0u:
            if ( subtrahend )
            {
                // 0 * anything - any_positive == -any_positive -> banned
                throw exception;
            }
            // ELSE: 0 * anything - 0 == 0 -> no change
            break;
        }

        this->clear_leading_zeros();
    }
    else if ( subtrahend )
    {
        // anything * 0 - any_positive == -any_positive -> banned
        throw exception;
    }
    else
    {
        // anything * 0 - 0 == 0
        this->digits_.clear();
    }

    BOOST_ASSERT( this->test_invariant() );
}

/** Amplifies the current number by a given (non-negative) value that is less
    than the radix.  It should be faster than converting the short multiplier to
    a full \c big_radix_whole\<\> before multiplying.

    \pre  <code>0 &lt;= <var>multiplier</var> &lt; Radix</code>

    \param multiplier  The multipiler to the current object

    \post  <code>*this == <var>old_this</var> * <var>multiplier</var></code>

    \see  #multiply_single_add_single
 */
template < int Radix, class Allocator >
inline
void
boost::math::big_radix_whole<Radix, Allocator>::multiply_single
(
    digit_type  multiplier
)
{
    this->multiply_single_add_single( multiplier, 0 );
}

/** Increases a pair of digits of the current number at a given place by the
    product of two (non-negative) values, with both factors less than the radix.
    It should be faster than converting either of the factors or the product to
    full \c big_radix_whole\<\> objects before (possibly multiplying, shifting,
    and) adding.

    \pre  <code>0 &lt;= <var>addend_multiplicand</var>,
          <var>addend_multiplier</var> &lt; Radix</code>
    \pre  <code><var>index</var> &lt; this-&gt;digit_limit() - 2</code>

    \param addend_multiplicand  The multiplicand in the product-addend
    \param addend_multiplier    The multiplier in the product-addend
    \param index                The place of the product-addend one's digit
                                during the addition

    \post  <code>*this == <var>old_this</var> + <var>addend_multiplicand</var> *
           <var>addend_multiplier</var> *
           Radix<sup><var>index</var></sup></code>
 */
template < int Radix, class Allocator >
void
boost::math::big_radix_whole<Radix, Allocator>::add_shifted_single_product
(
    digit_type  addend_multiplicand,
    digit_type  addend_multiplier,
    size_type   index
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    BOOST_ASSERT( 0 <= addend_multiplicand && addend_multiplicand <
     self_type::radix && 0 <= addend_multiplier && addend_multiplier <
     self_type::radix );
    BOOST_ASSERT( index < this->digits_.max_size() - 2u );  // extra for carry

    if ( digit_type const  product = addend_multiplicand * addend_multiplier )
    {
        // Determine the high and low digits of the 2-digit-max product
        std::div_t const  product_hl = std::div( product, self_type::radix );
        int const &       product_h = product_hl.quot;
        int const &       product_l = product_hl.rem;

        // Do the addition
        size_type const  s = this->digits_.size();

        if ( index >= s )
        {
            // Append the product as new digits
            this->digits_.insert( this->digits_.end(), index - s + 2u, 0 );
            this->digits_.back() = product_h;
            this->digits_[ index ] = product_l;
        }
        else if ( index == s - 1u )
        {
            // Append the high product digit, add the low one; due to the
            // constraints on the factors, there can't be a carry cascade
            this->digits_.push_back( product_h );
            if ( (this->digits_[ index ] += product_l) >= self_type::radix )
            {
                this->digits_[ index ] -= self_type::radix;
                ++this->digits_.back();
            }
        }
        else
        {
            // Spare digit space for cascading addition carry
            this->digits_.push_back( 0 );

            // Both product digits can be added directly
            iterator  i = this->digits_.begin() + index;

            if( (*i++ += product_l) >= self_type::radix )
            {
                *(i - 1) -= self_type::radix;
                ++*i;
            }

            *i += product_h;
            while ( *i++ >= self_type::radix )
            {
                *(i - 1) -= self_type::radix;
                ++*i;
            }
            BOOST_ASSERT( i <= this->digits_.end() );
        }

        // Remove unused highest-placed zeros
        this->clear_leading_zeros();
    }
    // ELSE: anything + 0 == anything -> no change

    BOOST_ASSERT( this->test_invariant() );
}

/** Increases the current number by the product of two (non-negative) values,
    with both factors less than the radix.  It should be faster than converting
    either of the factors or the product to full \c big_radix_whole\<\> objects
    before (possibly multiplying and) adding.

    \pre  <code>0 &lt;= <var>addend_multiplicand</var>,
          <var>addend_multiplier</var> &lt; Radix</code>

    \param addend_multiplicand  The multiplicand in the product-addend
    \param addend_multiplier    The multiplier in the product-addend

    \post  <code>*this == <var>old_this</var> + <var>addend_multiplicand</var> *
           <var>addend_multiplier</var></code>

    \see  #add_shifted_single_product
 */
template < int Radix, class Allocator >
inline
void
boost::math::big_radix_whole<Radix, Allocator>::add_single_product
(
    digit_type  addend_multiplicand,
    digit_type  addend_multiplier
)
{
    this->add_shifted_single_product( addend_multiplicand, addend_multiplier,
     0u );
}

/** Decreases a pair of digits of the current number at a given place by the
    product of two (non-negative) values, with both factors less than the radix.
    It should be faster than converting either of the factors or the product to
    full \c big_radix_whole\<\> objects before (possibly multiplying, shifting,
    and) subtracting.

    \pre  <code>0 &lt;= <var>subtrahend_multiplicand</var>,
          <var>subtrahend_multiplier</var> &lt; Radix</code>
    \pre  <code><var>index</var> &lt; this-&gt;digit_limit() - 1</code>

    \param subtrahend_multiplicand  The multiplicand in the product-subtrahend
    \param subtrahend_multiplier    The multiplier in the product-subtrahend
    \param index                    The place of the product-subtrahend's one's
                                    digit during the subtraction

    \throws boost::math::big_radix_whole_negative_result_error
             The current state describes a value less than the shifted
             product-subtrahend, which would require a negative value for the
             new state, which is not representable.

    \post  <code>*this == <var>old_this</var> -
           <var>subtrahend_multiplicand</var> *
           <var>subtrahend_multiplier</var> *
           Radix<sup><var>index</var></sup></code>
 */
template < int Radix, class Allocator >
void
boost::math::big_radix_whole<Radix, Allocator>::subtract_shifted_single_product
(
    digit_type  subtrahend_multiplicand,
    digit_type  subtrahend_multiplier,
    size_type   index
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    BOOST_ASSERT( 0 <= subtrahend_multiplicand && subtrahend_multiplicand <
     self_type::radix && 0 <= subtrahend_multiplier && subtrahend_multiplier <
     self_type::radix );
    BOOST_ASSERT( index < this->digits_.max_size() - 1u );

    if ( digit_type const  product = subtrahend_multiplicand *
     subtrahend_multiplier )
    {
        big_radix_whole_negative_result_error const  exception( "attempted to"
         " subtract a larger short-product (possibly after shifting)" );

        // Determine the high and low digits of the 2-digit-max product
        std::div_t const  product_hl = std::div( product, self_type::radix );
        int const &       product_h = product_hl.quot;
        int const &       product_l = product_hl.rem;

        // Do the subtraction
        size_type const  s = this->digits_.size();

        if ( index >= s )
        {
            // Must be too large
            throw exception;
        }
        else if ( index == s - 1u )
        {
            reference  top_digit = this->digits_.back();

            if ( product_h || (product_l > top_digit) )
            {
                // Too large
                throw exception;
            }
            else
            {
                top_digit -= product_l;
            }
        }
        else if ( index == s - 2u )
        {
            reference  u_digit = this->digits_.back(),
                      pu_digit = this->digits_[ index ];

            if ( product > (u_digit * self_type::radix + pu_digit) )
            {
                // Too large
                throw exception;
            }
            else
            {
                u_digit -= product_h;
                if ( (pu_digit -= product_l) < 0 )
                {
                    pu_digit += self_type::radix;
                    --u_digit;
                }
            }
        }
        else
        {
            // The product will always fit
            iterator  i = this->digits_.begin() + index;

            *i -= product_l;
            if ( 0 > *i )
            {
                *i += self_type::radix;
                --*(i + 1);
            }

            *++i -= product_h;
            while ( 0 > *i++ )
            {
                --*i;
                *(i - 1) += self_type::radix;
            }
            BOOST_ASSERT( i <= this->digits_.end() );
        }

        // Remove unused highest-placed zeros
        this->clear_leading_zeros();
    }
    // ELSE: anything - 0 == anything -> no change

    BOOST_ASSERT( this->test_invariant() );
}

/** Decreases the current number by the product of two (non-negative) values,
    with both factors less than the radix.  It should be faster than converting
    either of the factors or the product to full \c big_radix_whole\<\> objects
    before (possibly multiplying and) subtracting.

    \pre  <code>0 &lt;= <var>subtrahend_multiplicand</var>,
          <var>subtrahend_multiplier</var> &lt; Radix</code>

    \param subtrahend_multiplicand  The multiplicand in the product-subtrahend
    \param subtrahend_multiplier    The multiplier in the product-subtrahend

    \throws boost::math::big_radix_whole_negative_result_error
             The current state describes a value less than the
             product-subtrahend, which would require a negative value for the
             new state, which is not representable.

    \post  <code>*this == <var>old_this</var> -
           <var>subtrahend_multiplicand</var> *
           <var>subtrahend_multiplier</var></code>

    \see  #subtract_shifted_single_product
 */
template < int Radix, class Allocator >
inline
void
boost::math::big_radix_whole<Radix, Allocator>::subtract_single_product
(
    digit_type  subtrahend_multiplicand,
    digit_type  subtrahend_multiplier
)
{
    this->subtract_shifted_single_product( subtrahend_multiplicand,
     subtrahend_multiplier, 0u );
}

/** Replaces the current value with its absolute value after decreasing a pair
    of its digits, at a given place and the place immediately above it, by a
    product of two values (each one non-negative and less than the radix).  It
    should be faster than converting either factor, or their product, to full
    \c big_radix_whole\<\> objects before shifting/multiplying and subtracting
    (and doing the sign and magnitude checks).

    \pre  <code>0 &lt;= <var>subtrahend_multiplicand</var>,
          <var>subtrahend_multiplier</var> &lt; Radix</code>
    \pre  <code><var>index</var> &lt; this-&gt;digit_limit() - 1</code>

    \param subtrahend_multiplicand  The multiplicand in the product-subtrahend
    \param subtrahend_multiplier    The multiplier in the product-subtrahend
    \param index                    The place of the product-subtrahend's one's
                                    digit during the subtraction

    \retval true   The difference was originally negative (i.e.
                   <code><var>subtrahend_multiplicand</var> *
                   <var>subtrahend_multiplier</var> *
                   Radix<sup><var>index</var></sup> &gt; *this</code>).
    \retval false  The difference was originally non-negative.

    \post  <code>*this == |<var>old_this</var> -
           <var>subtrahend_multiplicand</var> *
           <var>subtrahend_multiplier</var> *
           Radix<sup><var>index</var></sup>|</code>
 */
template < int Radix, class Allocator >
bool
boost::math::big_radix_whole<Radix,
 Allocator>::subtract_shifted_single_product_absolutely
(
    digit_type  subtrahend_multiplicand,
    digit_type  subtrahend_multiplier,
    size_type   index
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    BOOST_ASSERT( 0 <= subtrahend_multiplicand && subtrahend_multiplicand <
     self_type::radix && 0 <= subtrahend_multiplier && subtrahend_multiplier <
     self_type::radix );
    BOOST_ASSERT( index < this->digits_.max_size() - 1u );

    bool  result = false;

    if ( digit_type const  product = subtrahend_multiplicand *
     subtrahend_multiplier )
    {
        // Determine the high and low digits of the 2-digit-max product
        std::div_t const  product_hl = std::div( product, self_type::radix );
        int const &       product_h = product_hl.quot;
        int const &       product_l = product_hl.rem;

        // Do the subtraction
        size_type  s = this->digits_.size();

        if ( index >= s )  // both digits outside block
        {
            // Append space for product digits and any intermediate zeros
            // (in one shot to reduce number of throw spots)
            this->digits_.insert( this->digits_.end(), index - s + 1u +
             static_cast<unsigned>(0u != product_h), 0 );

            // Add (actually subtract) the product's digits
            reference  back_digit = this->digits_.back();

            s = this->digits_.size();
            if ( product_h )
            {
                reference  next_to_back = this->digits_[ s - 2u ];

                back_digit = -product_h;
                next_to_back = -product_l;

                // Handle borrows
                while ( 0 > next_to_back )
                {
                    next_to_back += self_type::radix;
                    --back_digit;
                }
            }
            else
            {
                // Leave out zero-valued upper digit, just handle lower digit
                back_digit -= product_l;
            }
            BOOST_ASSERT( 0 > back_digit );
        }
        else if ( index + 1u == s )  // digits straddle block
        {
            // Append the new digit space for the subtracted higher digit
            if ( product_h )
            {
                this->digits_.push_back( -product_h );
                s = this->digits_.size();

                // Finish the subtraction
                reference  back_digit = this->digits_.back();
                reference  next_to_back = this->digits_[ s - 2u ];

                next_to_back -= product_l;

                // Handle borrows
                while ( 0 > next_to_back )
                {
                    next_to_back += self_type::radix;
                    --back_digit;
                }
                BOOST_ASSERT( 0 > back_digit );
            }
            else
            {
                // Don't append a zero digit, just handle the lower digit
                this->digits_.back() -= product_l;
                this->clear_leading_zeros();
            }
        }
        else  // both digits inside block
        {
            // Just do the subtraction
            this->digits_[ index + 1u ] -= product_h;
            this->digits_[ index ] -= product_l;

            // Handle borrows
            for ( size_type  i = index + 1u ; i < s ; ++i )
            {
                reference  current = this->digits_[ i ];
                reference  previous = this->digits_[ i - 1u ];

                while ( 0 > previous )
                {
                    previous += self_type::radix;
                    --current;
                }
            }
            this->clear_leading_zeros();
        }

        // A larger subtrahend gives a negative value in the highest place
        // (copied from member function "subtract_shifted_single_absolutely")
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
    // ELSE: anything - 0 == anything -> no change

    BOOST_ASSERT( this->test_invariant() );
    return result;
}

/** Replaces the current value with the absolute difference between that value
    and the product of two (non-negative) values, with both factors less than
    the radix.  It should be faster than converting either of the factors or
    their product to full \c big_radix_whole\<\> objects before subtracting.

    \pre  <code>0 &lt;= <var>subtrahend_multiplicand</var>,
          <var>subtrahend_multiplier</var> &lt; Radix</code>

    \param subtrahend_multiplicand  The multiplicand in the product-subtrahend
    \param subtrahend_multiplier    The multiplier in the product-subtrahend

    \retval true   The difference was originally negative (i.e.
                   <code><var>subtrahend_multiplicand</var> *
                   <var>subtrahend_multiplier</var> &gt; *this</code>).
    \retval false  The difference was originally non-negative.

    \post  <code>*this == |<var>old_this</var> -
           <var>subtrahend_multiplicand</var> *
           <var>subtrahend_multiplier</var>|</code>

    \see  #subtract_shifted_single_product_absolutely
 */
template < int Radix, class Allocator >
inline
bool
boost::math::big_radix_whole<Radix,
 Allocator>::subtract_single_product_absolutely
(
    digit_type  subtrahend_multiplicand,
    digit_type  subtrahend_multiplier
)
{
    return this->subtract_shifted_single_product_absolutely(
     subtrahend_multiplicand, subtrahend_multiplier, 0u );
}

/** Increases the current number's value by the product of the given values,
    each of a different type.  It should be faster than either converting the
    single-digit factor to a full \c big_radix_whole\<\> object before
    multiplying or doing the shifting separately from the primary
    multiplication before the adding.

    \pre  <code><b>Max</b>( this-&gt;digit_count(), <var>index</var> +
          <var>addend_full_factor</var>.digit_count() + 1 ) + 1 &lt;=
          this-&gt;digit_limit()</code>
    \pre  <code>0 &lt;= <var>addend_single_factor</var> &lt; Radix</code>

    \param addend_full_factor    The first factor in the product-addend
    \param addend_single_factor  The second factor in the product-addend
    \param index                 The place of the product-addend one's digit
                                 during the addition.  If not given, it defaults
                                 to zero (i.e. no shift).

    \post  <code>*this == <var>old_this</var> + <var>addend_full_factor</var> *
           <var>addend_single_factor</var> *
           Radix<sup><var>index</var></sup></code>
 */
template < int Radix, class Allocator >
void
boost::math::big_radix_whole<Radix, Allocator>::add_mixed_product
(
    big_radix_whole const &  addend_full_factor,
    digit_type               addend_single_factor,
    size_type                index  // = 0u
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );
    BOOST_PRIVATE_WILD_ASSERT( addend_full_factor.test_invariant() );

    deque_type const &  fd = addend_full_factor.digits_;
    deque_type &        td = this->digits_;
    size_type const     fs = fd.size(), ts = td.size(), tsm = td.max_size();

    BOOST_ASSERT( 0 <= addend_single_factor && addend_single_factor <
     self_type::radix );
    BOOST_ASSERT( (fs < tsm) && (index < tsm - fs) && (std::max( ts, fs + index
     + 1u ) <= tsm - 1u) );
        // tests are ordered to avoid under/over-flow

    if ( fs && addend_single_factor )
    {
        using namespace boost::lambda;

        // Allocate space for the product, and a carry for the addition
        size_type const  ps = fs + 1u + index;

        if ( ps > ts )
        {
            td.insert( td.end(), ps - ts + 1u, 0 );
        }
        else
        {
            td.push_back( 0 );
        }

        // Do the fused-multiply/add
        iterator const  tb = td.begin() + index;

        std::transform( fd.begin(), fd.end(), tb, tb, _2 + _1 *
         addend_single_factor );

        // Resolve carries
        iterator const  tp = td.end() - 1;

        for ( iterator  i = tb ; tp > i ; )
        {
            reference  current = *i, next = *++i;

            while ( self_type::radix <= current )
            {
                current -= self_type::radix;
                ++next;
            }
        }
        BOOST_ASSERT( self_type::radix > *tp );

        // Removed unused space (e.g. the digits involved were small)
        this->clear_leading_zeros();
    }
    // ELSE: anything + 0 == anything -> no change 

    BOOST_ASSERT( this->test_invariant() );
}

/** Decreases the current number's value by the product of the given values,
    each of a different type.  It should be faster than either converting the
    single-digit factor to a full \c big_radix_whole\<\> object before
    multiplying or doing the shifting separately from the primary
    multiplication before the subtracting.

    \pre  <code><var>subtrahend_full_factor</var> *
          <var>subtrahend_single_factor</var> * Radix<sup><var>index</var></sup>
          &lt;= *this</code>
    \pre  <code>0 &lt;= <var>subtrahend_single_factor</var> &lt; Radix</code>

    \param subtrahend_full_factor    The first factor in the product-subtrahend
    \param subtrahend_single_factor  The second factor in the product-subtrahend
    \param index                     The place of the product-subtrahend one's
                                     digit during the subtraction.  If not
                                     given, it defaults to zero (i.e. no shift).

    \throws boost::math::big_radix_whole_negative_result_error
             The current state describes a value less than the shifted
             product-subtrahend, which would require a negative value for the
             new state, which is not representable.

    \post  <code>*this == <var>old_this</var> -
           <var>subtrahend_full_factor</var> *
           <var>subtrahend_single_factor</var> *
           Radix<sup><var>index</var></sup></code>
 */
template < int Radix, class Allocator >
void
boost::math::big_radix_whole<Radix, Allocator>::subtract_mixed_product
(
    big_radix_whole const &  subtrahend_full_factor,
    digit_type               subtrahend_single_factor,
    size_type                index  // = 0u
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );
    BOOST_PRIVATE_WILD_ASSERT( subtrahend_full_factor.test_invariant() );

    deque_type const &  fd = subtrahend_full_factor.digits_;
    deque_type &        td = this->digits_;

    BOOST_ASSERT( 0 <= subtrahend_single_factor && subtrahend_single_factor <
     self_type::radix );

    if ( !fd.empty() && subtrahend_single_factor )
    {
        using namespace boost::lambda;
        using std::transform;

        big_radix_whole_negative_result_error const  exception( "attempted to "
         "subtract a larger mixed-product (possibly after shifting)" );

        // Take care of obviously oversized subtrahends
        size_type const  fs = fd.size(), ts = td.size();

        if ( (fs > ts) || (index >= ts) || (fs > ts - index) )
        {
            throw exception;
        }

        // Do the fused-multiply/subtract
        const_iterator const  fb = fd.begin(), fe = fd.end();
        iterator const        tb = td.begin() + index;

        transform( fb, fe, tb, tb, _2 - _1 * subtrahend_single_factor );

        // Resolve borrows
        iterator const  tp = td.end() - 1;

        for ( iterator  i = tb ; tp > i ; )
        {
            reference  current = *i, next = *++i;

            while ( 0 > current )
            {
                current += self_type::radix;
                --next;
            }
        }

        // If the product was larger than the existing number, then undo
        // everything (checking for non-obvious oversized products in advance
        // would take more resources than just doing it for real).
        reference  last = *tp;

        if ( 0 > last )
        {
            // Do a fused-multiply/add
            transform( fb, fe, tb, tb, _2 + _1 * subtrahend_single_factor );

            // Resolve carries
            for ( iterator  j = tb ; tp > j ; )
            {
                reference  current = *j, next = *++j;

                while ( self_type::radix <= current )
                {
                    current -= self_type::radix;
                    ++next;
                }
            }
            BOOST_ASSERT( self_type::radix > last && 0 < last );

            // Send notice
            throw exception;
        }
        else
        {
            // Remove cleared-out digits
            this->clear_leading_zeros();
        }
    }
    // ELSE: anything - 0 == anything -> no change

    BOOST_ASSERT( this->test_invariant() );
}

/** Replaces the current number's value with its absolute value taken after
    decreasing the value by the product of the given values, each of a different
    type.  It should be faster than either converting the single-digit factor to
    a full \c big_radix_whole\<\> object before multiplying or doing the
    shifting separately from the primary multiplication before the subtracting
    (and doing the sign and magnitude checks).

    \pre  <code><var>index</var> +
          <var>subtrahend_full_factor</var>.digit_count() + 1 &lt;=
          this-&gt;digit_limit()</code>
    \pre  <code>0 &lt;= <var>subtrahend_single_factor</var> &lt; Radix</code>

    \param subtrahend_full_factor    The first factor in the product-subtrahend
    \param subtrahend_single_factor  The second factor in the product-subtrahend
    \param index                     The place of the product-subtrahend one's
                                     digit during the subtraction.  If not
                                     given, it defaults to zero (i.e. no shift).

    \retval true   The difference was originally negative (i.e.
                   <code><var>subtrahend_full_factor</var> *
                   <var>subtrahend_single_factor</var> *
                   Radix<sup><var>index</var></sup> &gt; *this</code>).
    \retval false  The difference was originally non-negative.

    \post  <code>*this == |<var>old_this</var> -
           <var>subtrahend_full_factor</var> *
           <var>subtrahend_single_factor</var> *
           Radix<sup><var>index</var></sup>|</code>
 */
template < int Radix, class Allocator >
bool
boost::math::big_radix_whole<Radix,
 Allocator>::subtract_mixed_product_absolutely
(
    big_radix_whole const &  subtrahend_full_factor,
    digit_type               subtrahend_single_factor,
    size_type                index  // = 0u
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );
    BOOST_PRIVATE_WILD_ASSERT( subtrahend_full_factor.test_invariant() );

    deque_type const &  fd = subtrahend_full_factor.digits_;
    deque_type &        td = this->digits_;
    size_type const     fs = fd.size(), tsm = td.max_size();

    BOOST_ASSERT( 0 <= subtrahend_single_factor && subtrahend_single_factor <
     self_type::radix );
    BOOST_ASSERT( (fs < tsm) && (index <= tsm - fs - 1u) );
        // tests are ordered to avoid under/over-flow

    bool  result = false;

    if ( fs && subtrahend_single_factor )
    {
        using namespace boost::lambda;

        // Allocate space for the product
        size_type const  ts = td.size(), ss = fs + 1u + index;

        if ( ss > ts )
        {
            td.insert( td.end(), ss - ts, 0 );
        }

        // Do the fused-multiply/subtract
        iterator const  tb = td.begin() + index;

        std::transform( fd.begin(), fd.end(), tb, tb, _2 - _1 *
         subtrahend_single_factor );

        // Resolve borrows
        iterator const  tp = td.end() - 1;

        for ( iterator  i = tb ; tp > i ; )
        {
            reference  current = *i, next = *++i;

            while ( 0 > current )
            {
                current += self_type::radix;
                --next;
            }
        }

        // Remove cleared-out digits (invalidates "tb" and "tp")
        this->clear_leading_zeros();

        // Negate a negative result
        if (   (result = !td.empty()) && (0 > td.back()) )
        {
            iterator const  tp2 = td.end() - 1;
            reference       last = *tp2;

            for ( iterator  j = td.begin() ; tp2 > j ; )
            {
                reference  current = *j, next = *++j;

                current = -current;
                while ( 0 > current )
                {
                    current += self_type::radix;
                    ++next;  // negates to operator-- on next iteration
                }
            }
            last = -last;
            BOOST_ASSERT( 0 <= last );

            this->clear_leading_zeros();
        }
    }
    // ELSE: |anything - 0| == |anything| -> no change for non-negatives

    BOOST_ASSERT( this->test_invariant() );
    return result;
}

/** Increases the current number's value by the product of the given values
    starting at a given shift amount.  (Using the built-in shift should be
    faster than shifting an already-prepared product before adding).

    \pre  <code><b>Max</b>( this-&gt;digit_count(), <var>index</var> +
          <var>addend_multiplicand</var>.digit_count() +
          <var>addend_multiplier</var>.digit_count() ) + 1 &lt;=
          this-&gt;digit_limit()</code>

    \param addend_multiplicand  The multiplicand in the product-addend
    \param addend_multiplier    The multiplier in the product-addend
    \param index                The place of the product-addend one's digit
                                during the addition.

    \post  <code>*this == <var>old_this</var> + <var>addend_multiplicand</var> *
           <var>addend_multiplier</var> *
           Radix<sup><var>index</var></sup></code>
 */
template < int Radix, class Allocator >
void
boost::math::big_radix_whole<Radix, Allocator>::add_shifted_full_product
(
    big_radix_whole const &  addend_multiplicand,
    big_radix_whole const &  addend_multiplier,
    size_type                index
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );
    BOOST_PRIVATE_WILD_ASSERT( addend_multiplicand.test_invariant() );
    BOOST_PRIVATE_WILD_ASSERT( addend_multiplier.test_invariant() );

    deque_type const &  dd = addend_multiplicand.digits_;
    deque_type const &  rd = addend_multiplier.digits_;
    deque_type &        td = this->digits_;
    size_type const     ds = dd.size(), rs = rd.size(), ts = td.size(),
                        tsm = td.max_size();

    BOOST_ASSERT( (index < tsm) && (rs < tsm - index) && (ds < tsm - index - rs)
     && (std::max( ts, ds + rs + index ) <= tsm - 1u) );
        // tests are ordered to avoid under/over-flow

    if ( ds && rs )
    {
        // Reserve space for the product-addend and cascading carry
        size_type const  ps = ds + rs, sps = ps + index;

        if ( sps > ts )
        {
            td.insert( td.end(), sps - ts + 1u, 0 );
        }
        else
        {
            td.push_back( 0 );
        }

        // Do the huge partial-products loops, with fused adds and built-in
        // carry propagation
        const_iterator const  de = dd.end(), re = rd.end();
        iterator const        tp = td.end() - 1;
        iterator              tb0 = td.begin() + index;

        for ( const_iterator  di = dd.begin() ; de > di ; ++di )
        {
            iterator  tb1 = tb0++;

            // main fused-multiply/add
            for ( const_iterator  ri = rd.begin() ; re > ri ; ++ri )
            {
                reference    current = *tb1, next = *++tb1;
                std::div_t const  qr = std::div( current + (*di) * (*ri),
                 self_type::radix );

                current = qr.rem;
                next += qr.quot;
            }
            BOOST_ASSERT( tb1 <= tp );

            // resolve cascading carries
            while ( (tb1 < tp) && (self_type::radix <= *tb1) )
            {
                *tb1 -= self_type::radix;
                ++*++tb1;
            }
        }
        BOOST_ASSERT( self_type::radix > td.back() );

        // If partial products are small....
        this->clear_leading_zeros();
    }
    // ELSE: anything + 0 == anything -> no change

    BOOST_ASSERT( this->test_invariant() );
}

/** Increases the current number's value by the product of the given values.

    \pre  <code><b>Max</b>( this-&gt;digit_count(),
          <var>addend_multiplicand</var>.digit_count() +
          <var>addend_multiplier</var>.digit_count() ) + 1 &lt;=
          this-&gt;digit_limit()</code>

    \param addend_multiplicand  The multiplicand in the product-addend
    \param addend_multiplier    The multiplier in the product-addend

    \post  <code>*this == <var>old_this</var> + <var>addend_multiplicand</var> *
           <var>addend_multiplier</var></code>

    \see  #add_shifted_full_product
 */
template < int Radix, class Allocator >
inline
void
boost::math::big_radix_whole<Radix, Allocator>::add_full_product
(
    big_radix_whole const &  addend_multiplicand,
    big_radix_whole const &  addend_multiplier
)
{
    this->add_shifted_full_product(addend_multiplicand, addend_multiplier, 0u);
}

/** Decreases the current number's value by the product of the given values
    starting at a given shift amount.  (Using the built-in shift should be
    faster than shifting an already-prepared product before subtracting).

    \pre  <code><var>subtrahend_multiplicand</var> *
          <var>subtrahend_multiplier</var> * Radix<sup><var>index</var></sup>
          &lt;= *this</code>

    \param subtrahend_multiplicand  The multiplicand in the product-subtrahend
    \param subtrahend_multiplier    The multiplier in the product-subtrahend
    \param index                    The place of the product-subtrahend one's
                                    digit during the subtraction.

    \throws boost::math::big_radix_whole_negative_result_error
             The current state describes a value less than the shifted
             product-subtrahend, which would require a negative value for the
             new state, which is not representable.

    \post  <code>*this == <var>old_this</var> -
           <var>subtrahend_multiplicand</var> *
           <var>subtrahend_multiplier</var> *
           Radix<sup><var>index</var></sup></code>
 */
template < int Radix, class Allocator >
void
boost::math::big_radix_whole<Radix, Allocator>::subtract_shifted_full_product
(
    big_radix_whole const &  subtrahend_multiplicand,
    big_radix_whole const &  subtrahend_multiplier,
    size_type                index
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );
    BOOST_PRIVATE_WILD_ASSERT( subtrahend_multiplicand.test_invariant() );
    BOOST_PRIVATE_WILD_ASSERT( subtrahend_multiplier.test_invariant() );

    deque_type const &  dd = subtrahend_multiplicand.digits_;
    deque_type const &  rd = subtrahend_multiplier.digits_;
    deque_type &        td = this->digits_;

    if ( !dd.empty() && !rd.empty() )
    {
        big_radix_whole_negative_result_error const  exception( "attempted to "
         "subtract a larger full-product (possibly after shifting)" );

        // Take care of obviously oversized subtrahends, noting that n-digits
        // times m-digits is either n+m or n+m-1 digits
        size_type const  ds = dd.size(), rs = rd.size(), ts = td.size();

        if ( (index >= ts) || (ds > ts - index) || (rs > ts - index) ||
         (ds > ts - index - rs + 1u) )
        {
            throw exception;
        }

        // Do the fused multiply/subtract on a scratch copy  (It's not
        // feasible to do it in place and still keep undo-ability.)
        const_iterator const  de = dd.end(), re = rd.end();
        deque_type            cd( td );

        cd.push_back( 0 );  // so the loop's "next" always works

        iterator       cb0 = cd.begin() + index;
        iterator const  cp = cd.end() - 1;

        for ( const_iterator  di = dd.begin() ; de > di ; ++di )
        {
            iterator  cb1 = cb0++;

            // main fused-multiply/subtract
            for ( const_iterator  ri = rd.begin() ; re > ri ; ++ri )
            {
                reference    current = *cb1, next = *++cb1;
                std::div_t const  qr = std::div( current - (*di) * (*ri),
                 self_type::radix );

                current = qr.rem;
                next += qr.quot;

                // Since integer-division rounds to either 0 or -Infinity ...
                while ( 0 > current )
                {
                    current += self_type::radix;
                    --next;
                }
            }
            BOOST_ASSERT( cb1 <= cp );

            // resolve cascading borrows
            while ( (cb1 < cp) && (0 > *cb1) )
            {
                *cb1 += self_type::radix;
                --*++cb1;
            }
        }

        // Remove cleared peak digits to ease comparison
        while ( !cd.empty() && !cd.back() )
        {
            cd.pop_back();
        }

        // Retain only non-negative results
        if ( cd.empty() || (cd.back() > 0) )
        {
            cd.swap( td );
        }
        else
        {
            throw exception;
        }
    }
    // ELSE: anything - 0 == anything -> no change

    BOOST_ASSERT( this->test_invariant() );
}

/** Decreases the current number's value by the product of the given values.

    \pre  <code><var>subtrahend_multiplicand</var> *
          <var>subtrahend_multiplier</var> &lt;= *this</code>

    \param subtrahend_multiplicand  The multiplicand in the product-subtrahend
    \param subtrahend_multiplier    The multiplier in the product-subtrahend

    \throws boost::math::big_radix_whole_negative_result_error
             The current state describes a value less than the shifted
             product-subtrahend, which would require a negative value for the
             new state, which is not representable.

    \post  <code>*this == <var>old_this</var> -
           <var>subtrahend_multiplicand</var> *
           <var>subtrahend_multiplier</var></code>

    \see  #subtract_shifted_full_product
 */
template < int Radix, class Allocator >
inline
void
boost::math::big_radix_whole<Radix, Allocator>::subtract_full_product
(
    big_radix_whole const &  subtrahend_multiplicand,
    big_radix_whole const &  subtrahend_multiplier
)
{
    this->subtract_shifted_full_product( subtrahend_multiplicand,
     subtrahend_multiplier, 0u );
}

/** Replaces the current number's value with its absolute value taken after
    decreasing the value by the product of the given values starting at a given
    shift amount.  (Using the built-in shift should be faster than shifting an
    already-prepared product before subtracting).

    \pre  <code><var>index</var> +
          <var>subtrahend_multiplicand</var>.digit_count() +
          <var>subtrahend_multiplier</var>.digit_count() &lt;=
          this-&gt;digit_limit()</code>

    \param subtrahend_multiplicand  The multiplicand in the product-subtrahend
    \param subtrahend_multiplier    The multiplier in the product-subtrahend
    \param index                    The place of the product-subtrahend one's
                                    digit during the subtraction.

    \retval true   The difference was originally negative (i.e.
                   <code><var>subtrahend_multiplicand</var> *
                   <var>subtrahend_multiplier</var> *
                   Radix<sup><var>index</var></sup> &gt; *this</code>).
    \retval false  The difference was originally non-negative.

    \post  <code>*this == |<var>old_this</var> -
           <var>subtrahend_multiplicand</var> *
           <var>subtrahend_multiplier</var> *
           Radix<sup><var>index</var></sup>|</code>
 */
template < int Radix, class Allocator >
bool
boost::math::big_radix_whole<Radix,
 Allocator>::subtract_shifted_full_product_absolutely
(
    big_radix_whole const &  subtrahend_multiplicand,
    big_radix_whole const &  subtrahend_multiplier,
    size_type                index
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );
    BOOST_PRIVATE_WILD_ASSERT( subtrahend_multiplicand.test_invariant() );
    BOOST_PRIVATE_WILD_ASSERT( subtrahend_multiplier.test_invariant() );

    deque_type const &  dd = subtrahend_multiplicand.digits_;
    deque_type const &  rd = subtrahend_multiplier.digits_;
    deque_type &        td = this->digits_;
    size_type const     ds = dd.size(), rs = rd.size(), tsm = td.max_size();

    BOOST_ASSERT( (index < tsm) && (rs <= tsm - index) && (ds <= tsm - index -
     rs) );
        // tests are ordered to avoid under/over-flow

    bool  result = false;

    if ( ds && rs )
    {
        // Reserve space for the product-subtrahend, plus a spare
        size_type const  ts = td.size(), ps = ds + rs, sps = ps + index;

        if ( sps >= ts )
        {
            td.insert( td.end(), sps - ts + 1u, 0 );
        }

        // Do the huge partial-products loops, with fused subtracts and built-in
        // borrow propagation
        const_iterator const  de = dd.end(), re = rd.end();
        iterator const        tp = td.end() - 1;
        iterator              tb0 = td.begin() + index;

        for ( const_iterator  di = dd.begin() ; de > di ; ++di )
        {
            iterator  tb1 = tb0++;

            // main fused-multiply/subtract
            for ( const_iterator  ri = rd.begin() ; re > ri ; ++ri )
            {
                reference    current = *tb1, next = *++tb1;
                std::div_t const  qr = std::div( current - (*di) * (*ri),
                 self_type::radix );

                current = qr.rem;
                next += qr.quot;

                // Since integer-division rounds to either 0 or -Infinity ...
                while ( 0 > current )
                {
                    current += self_type::radix;
                    --next;
                }
            }
            BOOST_ASSERT( tb1 <= tp );

            // resolve cascading borrows
            while ( (tb1 < tp) && (0 > *tb1) )
            {
                *tb1 += self_type::radix;
                --*++tb1;
            }
        }

        // Remove cleared peak digits to ease comparison
        this->clear_leading_zeros();

        // Negate the result if necessary
        if ( result = (!td.empty() && ( 0 > td.back() )) )
        {
            // All the below-peak digits become negative...
            iterator const  tb = td.begin(), tp2 = td.end() - 1;

            for ( iterator  ti = tb ; tp2 > ti ; )
            {
                reference  current = *ti, next = *++ti;

                current = -current;
                while ( 0 > current )
                {
                    current += self_type::radix;
                    ++next;  // following iteration turns this into "--current"
                }
            }

            // ...and the peak one becomes positive
            reference  last = *tp2;

            last = -last;

            // If results shrink the digit count further....
            this->clear_leading_zeros();
        }
    }
    // ELSE: |anything - 0| == |anything| -> no change for non-negatives

    BOOST_ASSERT( this->test_invariant() );
    return result;
}

/** Replaces the current number's value with its absolute value taken after
    decreasing the value by the product of the given values.

    \pre  <code><var>subtrahend_multiplicand</var>.digit_count() +
          <var>subtrahend_multiplier</var>.digit_count() &lt;=
          this-&gt;digit_limit()</code>

    \param subtrahend_multiplicand  The multiplicand in the product-subtrahend
    \param subtrahend_multiplier    The multiplier in the product-subtrahend

    \retval true   The difference was originally negative (i.e.
                   <code><var>subtrahend_multiplicand</var> *
                   <var>subtrahend_multiplier</var> &gt; *this</code>).
    \retval false  The difference was originally non-negative.

    \post  <code>*this == |<var>old_this</var> -
           <var>subtrahend_multiplicand</var> *
           <var>subtrahend_multiplier</var>|</code>

    \see  #subtract_shifted_full_product_absolutely
 */
template < int Radix, class Allocator >
inline
bool
boost::math::big_radix_whole<Radix, Allocator>::subtract_full_product_absolutely
(
    big_radix_whole const &  subtrahend_multiplicand,
    big_radix_whole const &  subtrahend_multiplier
)
{
    return this->subtract_shifted_full_product_absolutely(
     subtrahend_multiplicand, subtrahend_multiplier, 0u );
}


//  Radix/bignum/natural operator member function definitions  ---------------//

/** Amplifies the current number by a given value.

    \param multiplier  The value to be multiplied to the current number, which
                       serves as the multiplicand.  (Don't make it too large, to
                       avoid memory problems.)

    \return  A reference to <code>*this</code> object as the product.

    \post  <code>*this == <var>old_this</var> <b>*</b>
           <var>multiplier</var></code>

    \note  The class declaration uses the Boost.Operators library to synthesize
           the (binary) multiplication <code>operator *</code> from this one.

    \see  #add_full_product
 */
template < int Radix, class Allocator >
boost::math::big_radix_whole<Radix, Allocator> &
boost::math::big_radix_whole<Radix, Allocator>::operator *=
(
    big_radix_whole const &  multiplier
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );
    BOOST_PRIVATE_WILD_ASSERT( multiplier.test_invariant() );

    // For now, use the long multiplication method, which is the same as fused
    // add & multiply with zero as the non-product addend.  Later, maybe
    // something "kewl" like Karatsuba, Toom-Cook, and/or Fourier tranform
    // methods (FFT, NTT, etc.) can be used.
    self_type  other( 0u, this->get_allocator() );

    other.add_full_product( *this, multiplier );
    this->swap( other );

    BOOST_ASSERT( this->test_invariant() );
    return *this;
}


//  Radix/bignum/natural miscellaneous function definitions  -----------------//

// Put something here?


#endif  // BOOST_MATH_BIG_RADIX_WHOLE_IMPL_MULTIPLICATION_IPP

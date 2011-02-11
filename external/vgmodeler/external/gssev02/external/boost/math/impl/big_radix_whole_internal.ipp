//  Boost math/impl/big_radix_whole_internal.ipp header file  ----------------//

//  Copyright 2006 Daryle Walker.  Distributed under the Boost Software License,
//  Version 1.0.  (See the accompanying file LICENSE_1_0.txt or a copy at
//  <http://www.boost.org/LICENSE_1_0.txt>.)

//  See <http://www.boost.org/libs/math/> for the library's home page.

/** \file
    \brief  Part of the implementation for an arbitrary-length specific-radix
            unsigned integer class template

    Contains the definitions of the private member functions of the
    \c boost::math::big_radix_whole\<\> class template.  These member functions
    are used to check and maintain invariants.
 */

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_IMPL_INTERNAL_IPP
#define BOOST_MATH_BIG_RADIX_WHOLE_IMPL_INTERNAL_IPP

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_CORE_HPP
#error "#Include only as part of <boost/math/big_radix_whole_core.hpp>"
#endif

#include <algorithm>  // for std::find_if
#include <cstdlib>    // for std::div, std::div_t

#include <boost/lambda/lambda.hpp>  // for boost::lamda::_1


//  Radix/bignum/natural state-checking member function definitions  ---------//

// Confirm that there are no leading zeros and all digits are in range
template < int Radix, class Allocator >
inline
bool
boost::math::big_radix_whole<Radix, Allocator>::test_invariant () const
{
    using namespace boost::lambda;

    return (this->digits_.empty() || 
            (0 != this->digits_.back()  && this->digits_.end() == 
             std::find_if( this->digits_.begin(), this->digits_.end(), _1 < 0 || _1 >= self_type::radix )) 
       );
}

// Ensure that there are no leading zeros
template < int Radix, class Allocator >
inline
void
boost::math::big_radix_whole<Radix, Allocator>::clear_leading_zeros
(
)
{
    while ( !this->digits_.empty() && !this->digits_.back() )
    {
        this->digits_.pop_back();
    }
}

// Normalize the digit string to match the invariant
template < int Radix, class Allocator >
void
boost::math::big_radix_whole<Radix, Allocator>::normalize
(
)
{
    using std::div;

    // Adjust each digit so |digit| < Radix, from the highest digit to the
    // lowest.  (The other way could lead to a carry bringing a very large
    // neighbor digit over the limit.)  The digit string is extended as needed.
    for ( size_type  i = this->digits_.size() ; i > 0 ; --i )
    {
        size_type const  current_size = this->digits_.size();
        std::div_t       qr = { 0, 0 };

        // Shift out the carry for a digit, remembering to propagate
        // carries to higher digits if there's a cascade.
        for ( size_type  j = i - 1u ; j < current_size ; ++j )
        {
            reference  current_element = this->digits_[ j ];

            qr = div( current_element + qr.quot, self_type::radix );
            current_element = qr.rem;

            if ( !qr.quot )
            {
                break;
            }
        }

        // Create digits for a remaining carry (the previous quotient is
        // non-zero only if that loop ended because the digit-string ended)
        while ( qr.quot )
        {
            qr = div( qr.quot, self_type::radix );
            this->digits_.push_back( qr.rem );
        }
    }

    // Remove any negative digits (i.e. Radix < Digit < 0)
    switch ( size_type const  s = this->digits_.size() )
    {
    default:
        // Sift out borrows
        for ( size_type  i = 1u ; s > i ; ++i )
        {
            reference  previous_element = this->digits_[ i - 1u ];

            if ( 0 > previous_element )
            {
                previous_element += self_type::radix;
                --this->digits_[ i ];
            }
        }
        // FALL THROUGH

    case 1u:
        // Outstanding borrow -> had negative value
        if ( 0 > this->digits_.back() )
        {
            throw big_radix_whole_negative_result_error( "negative value "
             "entered" );
        } 
        // FALL THROUGH

    case 0u:
        // Do nothing
        break;
    }

    // Remove any leading zeros
    this->clear_leading_zeros();
}


#endif  // BOOST_MATH_BIG_RADIX_WHOLE_IMPL_INTERNAL_IPP

//  Boost math/impl/big_radix_whole_assign.ipp header file  ------------------//

//  Copyright 2006 Daryle Walker.  Distributed under the Boost Software License,
//  Version 1.0.  (See the accompanying file LICENSE_1_0.txt or a copy at
//  <http://www.boost.org/LICENSE_1_0.txt>.)

//  See <http://www.boost.org/libs/math/> for the library's home page.

/** \file
    \brief  Part of the implementation for an arbitrary-length specific-radix
            unsigned integer class template

    Contains the definitions some of the public member functions of the
    \c boost::math::big_radix_whole\<\> class template, and some related
    non-member functions.  All of these (member) functions perform specialized
    assignments.
 */

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_IMPL_ASSIGN_IPP
#define BOOST_MATH_BIG_RADIX_WHOLE_IMPL_ASSIGN_IPP

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_CORE_HPP
#error "#Include only as part of <boost/math/big_radix_whole_core.hpp>"
#endif

#include <string>  // for std::string

#include <boost/assert.hpp>   // for BOOST_ASSERT
#include <boost/cstdint.hpp>  // for boost::uintmax_t


//  Radix/bignum/natural special-assignment member function definitions  -----//

/** Exchanges the state of the current object with the state of another
    \c big_radix_whole\<\> object.  This leaves each object equivalent to the
    other object's old state.

    \param other  The object that will exchange state with this one.

    \post  <code>*this == <var>old_other</var> &amp;&amp; <var>old_this</var> ==
            <var>other</var></code>
 */
template < int Radix, class Allocator >
inline
void
boost::math::big_radix_whole<Radix, Allocator>::swap
(
    big_radix_whole &  other
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );
    BOOST_PRIVATE_WILD_ASSERT( other.test_invariant() );

    this->digits_.swap( other.digits_ );

    BOOST_ASSERT( other.test_invariant() );
    BOOST_ASSERT( this->test_invariant() );
}

/** Resets the state of the current object to be equivalent to zero.  It is like
    default construction.

    \post  <code>0 == this-&gt;digit_count()</code>
    \post  <code>0 == this-&gt;digit_at(<var>i</var>)</code> for all
           <var>i</var>.
 */
template < int Radix, class Allocator >
inline
void
boost::math::big_radix_whole<Radix, Allocator>::reset
(
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    this->digits_.clear();

    BOOST_ASSERT( this->test_invariant() );
}

/** Resets the state of the current object to be equivalent to the given number.
    It should be faster and more conservative with memory than the equivalent
    #assign(uintmax_t) member function.

    \pre  <code>0 &lt;= <var>value</var> &lt; Radix</code>

    \param value  The new numerical value of this object.

    \post  <code>this-&gt;digit_count() &lt;= 1 &amp;&amp; this-&gt;digit_at(0)
           == <var>value</var></code>
 */
template < int Radix, class Allocator >
void
boost::math::big_radix_whole<Radix, Allocator>::assign_single
(
    digit_type  value
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    BOOST_ASSERT( 0 <= value && value < self_type::radix );

    if ( value )
    {
        if ( this->digits_.empty() )
        {
            // Allocate the sole digit
            this->digits_.push_back( value );
        }
        else
        {
            // Remove any excess digits before replacing digit
            this->digits_.erase(this->digits_.begin() + 1, this->digits_.end());
            this->digits_.front() = value;
        }
    }
    else
    {
        // Ensure a zero value
        this->digits_.clear();
    }

    BOOST_ASSERT( this->test_invariant() );
}

/** Resets the state of the current object to be equivalent to the given number.
    It is like the (built-in) integer conversion constructor.

    \param value  The new numerical value of this object.

    \post  <code>*this == big_radix_whole( <var>value</var>,
           this-&gt;get_allocator() )</code>

    \see  #big_radix_whole(uintmax_t,allocator_type const&)
 */
template < int Radix, class Allocator >
inline
void
boost::math::big_radix_whole<Radix, Allocator>::assign
(
    uintmax_t  value
)
{
    big_radix_whole( value, this->get_allocator() ).swap( *this );
}

/** Resets the state of the current object to be equivalent to the given object.
    It is like the (implicit) copy-assignment operator.

    \param value  The new numerical value of this object.

    \post  <code>*this == <var>value</var></code>
 */
template < int Radix, class Allocator >
inline
void
boost::math::big_radix_whole<Radix, Allocator>::assign
(
    big_radix_whole const &  value
)
{
    *this = value;
}

/** Resets the state of the current object to be equivalent to the given object.
    It is like the cross-version converting copy constructor.

    \param value  The new numerical value of this object.

    \post  <code>*this == big_radix_whole( <var>value</var>,
           this-&gt;get_allocator() )</code>

    \see  #big_radix_whole(big_radix_whole<Radix2,Allocator2> const&,allocator_type const&)
 */
template < int Radix, class Allocator >
template < int Radix2, class Allocator2 >
inline
void
boost::math::big_radix_whole<Radix, Allocator>::assign
(
    big_radix_whole<Radix2, Allocator2> const &  value
)
{
    big_radix_whole( value, this->get_allocator() ).swap( *this );
}

/** Resets the state of the current object to be equivalent to a number built
    from the given digit list.  It is like the digit-string constructor.

    \param begin  An iterator to the first element of the new digit list.  That
                  starting digit represents the one's place of the number, with
                  higher places occurring in order.  If the list is empty, then
                  this argument must equal the \p end one.
    \param end    An iterator to one-past-the-end of the new digit list.  It
                  must be reachable from \p begin through a finite number of
                  increments.

    \post  <code>*this == big_radix_whole( <var>begin</var>, <var>end</var>,
           this-&gt;get_allocator() )</code>

    \see  #big_radix_whole(InputIterator,InputIterator,allocator_type const&)
 */
template < int Radix, class Allocator >
template < typename InputIterator >
inline
void
boost::math::big_radix_whole<Radix, Allocator>::configure
(
    InputIterator  begin,
    InputIterator  end
)
{
    big_radix_whole( begin, end, this->get_allocator() ).swap( *this );
}

/** Resets the state of the current object to be equivalent to a number built
    from the given digit list string.  It is like the digits-in-C-string
    constructor.

    \param digit_text  The string to be interpreted as the new number.  The
                       number must take up the entire string.  It may begin with

                       a '+' or '-' sign.  The actual digits are '0'-'9',
                       'a'-'z', or 'A'-'Z', with letters representing values
                       10-35.  If there are no hyphens within the string (not
                       including an initial '-'), then each digit is translated
                       into a digit for the final number.  Otherwise, each
                       hyphen-separated group is taken as a decimal number, and
                       the group's value is used as a digit for the final
                       number.  Empty groups are not allowed.  The digits at the
                       beginning of the string are the highest places, with the
                       lower places proceeding in order.  An empty string is
                       interpreted as zero.  Any other character, including a
                       '+' outside the first position, is an error.

    \throws boost::math::big_radix_whole_bad_format_error
             The <var>digit_text</var> string has an improper format.
    \throws boost::math::big_radix_whole_negative_result_error
             The <var>digit_text</var> string describes a value less than zero.

    \post  <code>*this == big_radix_whole( std::string(<var>digit_text</var>),
           this-&gt;get_allocator() )</code>

    \see  #big_radix_whole(std::string const&,allocator_type const&)
 */
template < int Radix, class Allocator >
inline
void
boost::math::big_radix_whole<Radix, Allocator>::configure
(
    char const* digit_text
)
{
   // [RH][change] .. pulled out the testring
   //
   std::string  teststring(digit_text);
   big_radix_whole( teststring, this->get_allocator() ).swap(*this );
}


//  Radix/bignum/natural miscellaneous function definitions  -----------------//

/** \brief  Non-member swapping function for \c big_radix_whole\<\>

    Exchanges the states of two \c big_radix_whole\<\> objects.  This
    specialization of the algorithm can be called by generic code that uses
    free-function (template) swap assisted with Koenig lookup.

    \param a  The first object involved in the swap.
    \param b  The second object involved in the swap.

    \post  <code><var>a</var> == <var>old_b</var> &amp;&amp; <var>old_a</var> ==
           <var>b</var></code>

    \see  boost::math::big_radix_whole::swap

    \relates  boost::math::big_radix_whole
 */
template < int Radix, class Allocator >
inline
void
boost::math::swap
(
    boost::math::big_radix_whole<Radix, Allocator> &  a,
    boost::math::big_radix_whole<Radix, Allocator> &  b
)
{
    a.swap( b );
}


#endif  // BOOST_MATH_BIG_RADIX_WHOLE_IMPL_ASSIGN_IPP

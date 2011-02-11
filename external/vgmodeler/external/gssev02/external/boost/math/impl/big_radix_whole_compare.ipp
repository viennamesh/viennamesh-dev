//  Boost math/impl/big_radix_whole_compare.ipp header file  -----------------//

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
    operators, perform various comparisons.
 */

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_IMPL_COMPARE_IPP
#define BOOST_MATH_BIG_RADIX_WHOLE_IMPL_COMPARE_IPP

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_CORE_HPP
#error "#Include only as part of <boost/math/big_radix_whole_core.hpp>"
#endif

#include <algorithm>  // for std::mismatch
#include <cstddef>    // for NULL
#include <utility>    // for std::pair

#include <boost/assert.hpp>  // for BOOST_ASSERT


//  Radix/bignum/natural extra member function definitions  ------------------//

/** Replaces the current value to whatever Boolean value it maps to.  Zero maps
    to itself (i.e. <code>false</code>).  Other, nonzero, values map to one
    (i.e. <code>true</code>).

    \post  <code>*this == big_radix_whole(
           static_cast&lt;bool&gt;(<var>old_this</var>),
           this-&gt;get_allocator() )</code>
 */
template < int Radix, class Allocator >
void
boost::math::big_radix_whole<Radix, Allocator>::verify_self
(
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    if ( !this->digits_.empty() )
    {
        // non-zero maps to one as true
        this->digits_.erase( 1 + this->digits_.begin(), this->digits_.end() );
        this->digits_.front() = 1;
    }
    // ELSE: zero maps to zero as false -> no change needed

    BOOST_ASSERT( this->test_invariant() );
}

/** Replaces the current value to the logical inverse of whatever Boolean value
    it maps to.  Zero maps to one (i.e. <code>true</code>).  Other, nonzero,
    values map to zero (i.e. <code>false</code>).

    \post  <code>*this == big_radix_whole( not
           static_cast&lt;bool&gt;(<var>old_this</var>),
           this-&gt;get_allocator() )</code>
 */
template < int Radix, class Allocator >
void
boost::math::big_radix_whole<Radix, Allocator>::invert_self
(
)
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    if ( this->digits_.empty() )
    {
        // FALSE to TRUE -> zero to one
        this->digits_.push_front( 1 );
    }
    else
    {
        // TRUE to FALSE -> one (or any other nonzero) to zero
        this->digits_.clear();
    }

    BOOST_ASSERT( this->test_invariant() );
}

/** Compares the value of current object with another \c big_radix_whole\<\>
    object's value for their relative order.  The ordering is a total order.

    \param r  The other object for comparison

    \retval +1  <code>*this</code> has a greater value than \p r
    \retval  0  <code>*this</code> and \p r have the same value (i.e. equal)
    \retval -1  <code>*this</code> has a lesser value than \p r
 */
template < int Radix, class Allocator >
int
boost::math::big_radix_whole<Radix, Allocator>::compare
(
    big_radix_whole const &  r
) const
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );
    BOOST_PRIVATE_WILD_ASSERT( r.test_invariant() );

    size_type const  ts = this->digits_.size(), rs = r.digits_.size();

    if ( ts == rs )
    {
        const_reverse_iterator const  te = this->digits_.rend();

        std::pair<const_reverse_iterator, const_reverse_iterator> const  c =
         std::mismatch( this->digits_.rbegin(), te, r.digits_.rbegin() );

        return ( c.first == te ) ? 0 : ( *c.first < *c.second ) ? -1 : +1;
    }
    else
    {
        return ( ts < rs ) ? -1 : +1;
    }
}


//  Radix/bignum/natural operator member function definitions  ---------------//

/** Performs a conversion of this object's value to a Boolean one.  It can be
    used for quick (non)zero comparisons, especially as a declaration condition
    within if/switch/for/while statements.

    \retval true   <code>*this</code> represents a nonzero value.
    \retval false  <code>*this</code> represents a zero value.

    \note  A type besides <code>bool</code> is used as the destination type to
           prevent integer conversions.  It most likely is a pointer-to-member
           type, using <code>NULL</code> and an unexploitable pointer for the
           logic values.

    \see  #verify_self
 */

// [RH] 
// [TODO] check this 

template < int Radix, class Allocator >
inline
boost::math::big_radix_whole<Radix, Allocator>::operator bool_type
(
) const
{
     BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

     return this->digits_.empty() ? NULL : &dummy::d;
 }
 /*
// [RH] new bool operator
//
template < int Radix, class Allocator >
inline
boost::math::big_radix_whole<Radix, Allocator>::operator bool
(
) const
{
     BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

     return this->digits_.empty() ;
 }
*/

/** Compares the current object with another for equivalent values.  Such
    objects are equal if their digit strings have the same length with the same
    digits in the same order.

    \param rhs  The right-side operand to be compared.

    \retval true   \c *this and \p rhs are equivalent.
    \retval false  \c *this and \p rhs are not equivalent.

    \note  The class declaration uses the Boost.Operators library to synthesize
           <code>operator !=</code> from this operator.

    \see  #compare
 */
template < int Radix, class Allocator >
inline
bool
boost::math::big_radix_whole<Radix, Allocator>::operator ==
(
    big_radix_whole const &  rhs
) const
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );
    BOOST_PRIVATE_WILD_ASSERT( rhs.test_invariant() );

    return this->digits_ == rhs.digits_;
}

/** Compares the current object with another for their relative order.  One
    object is less than another if and only if either the first object has a
    shorter digit-list or the two digit-lists are the same length but the first
    object's list lexicographically compares to be less than the second object's
    list (with the comparison starting at the highest place going down).

    \param rhs  The right-side operand to be compared.

    \retval true   \c *this is less than \p rhs
    \retval false  \c *this is not less than \p rhs (either equal or greater
                   than)

    \note  The class declaration uses the Boost.Operators library to synthesize
           <code>operator &gt;</code>, <code>operator &lt;=</code>, and
           <code>operator &gt;=</code> from this operator.

    \see  #compare
 */
template < int Radix, class Allocator >
inline
bool
boost::math::big_radix_whole<Radix, Allocator>::operator <
(
    big_radix_whole const &  rhs
) const
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );
    BOOST_PRIVATE_WILD_ASSERT( rhs.test_invariant() );

    size_type const  ts = this->digits_.size(), rs = rhs.digits_.size();

    return (  ( ts < rs ) || 
              ( ( ts == rs ) && std::lexicographical_compare( this->digits_.rbegin(), this->digits_.rend(), rhs.digits_.rbegin(), rhs.digits_.rend() ) )
       );
}


//  Radix/bignum/natural non-member operator function definitions  -----------//

/** \brief  Logical-not operator for \c big_radix_whole\<\>

    Checks the logical state of a \c big_radix_whole\<\> object and returns the
    complementary value.  Zero values originally map to <code>false</code> while
    every other value maps to <code>true</code>, so this operator switches those
    two mappings.

    \param x  The operand to be logically-inverted.

    \retval true   \p x is zero.
    \retval false  \p x is not zero.

    \see  boost::math::big_radix_whole::invert_self

    \relates  boost::math::big_radix_whole
 */
template < int Radix, class Allocator >
inline
bool
boost::math::operator !
(
    big_radix_whole<Radix, Allocator> const &  x
)
{
    return !static_cast<bool>( x );
}


#endif  // BOOST_MATH_BIG_RADIX_WHOLE_IMPL_COMPARE_IPP

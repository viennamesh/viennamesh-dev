//  Boost math/impl/big_radix_whole_accessor.ipp header file  ----------------//

//  Copyright 2006 Daryle Walker.  Distributed under the Boost Software License,
//  Version 1.0.  (See the accompanying file LICENSE_1_0.txt or a copy at
//  <http://www.boost.org/LICENSE_1_0.txt>.)

//  See <http://www.boost.org/libs/math/> for the library's home page.

/** \file
    \brief  Part of the implementation for an arbitrary-length specific-radix
            unsigned integer class template

    Contains the definitions some of the public member functions of the
    \c boost::math::big_radix_whole\<\> class template.  These member functions
    inspect the basic state of a \c big_radix_whole object.
 */

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_IMPL_ACCESSOR_IPP
#define BOOST_MATH_BIG_RADIX_WHOLE_IMPL_ACCESSOR_IPP

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_CORE_HPP
#error "#Include only as part of <boost/math/big_radix_whole_core.hpp>"
#endif


//  Radix/bignum/natural accessor member function definitions  ---------------//

/** Gets the number of digits allocated for this bignum object.

    \return  Let \p x be the represented number.  If \p x is zero, then return
             zero.  Otherwise, return <code>1 + floor( log{Radix}(x) )</code>.
 */
template < int Radix, class Allocator >
inline
typename boost::math::big_radix_whole<Radix, Allocator>::size_type
boost::math::big_radix_whole<Radix, Allocator>::digit_count
(
) const
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    return this->digits_.size();
}

/** Gets the value of a particular digit for this bignum object.  It should be
    slightly quicker since bounds checking is not done.  However, the \p index
    \e must be in range.

    \pre  <code><var>index</var> &lt; this-&gt;digit_count()</code>

    \param index  The particular power of the radix to get the coefficient of.

    \return  <code>this-&gt;digit_at( <var>index</var> )</code>

    \see  #digit_at
 */
template < int Radix, class Allocator >
inline
typename boost::math::big_radix_whole<Radix, Allocator>::digit_type
boost::math::big_radix_whole<Radix, Allocator>::quick_digit_at
(
    size_type  index
) const
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    BOOST_ASSERT( index < this->digit_count() );

    return this->digits_[ index ];
}

/** Gets the value of a particular digit for this bignum object.

    \param index  The particular power of the radix to get the coefficient of.

    \return  Let \p x be the represented number.  Return <code>(x <b>div</b>
             Radix<sup><var>index</var></sup>) <b>mod</b> Radix</code>.
 */
template < int Radix, class Allocator >
inline
typename boost::math::big_radix_whole<Radix, Allocator>::digit_type
boost::math::big_radix_whole<Radix, Allocator>::digit_at
(
    size_type  index
) const
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    // If BOOST_ASSERT is active, then the call to quick_digit_at repeats the
    // test already done here.  But it's a trade-off, do I repeat the test, or
    // do I repeat the indexing expression, risking that the two digit-at
    // methods get out of sync?
    return ( index < this->digit_count() ) ? this->quick_digit_at( index ) : 0;
}

/** Gets the allocation method used for this bignum object.

    \return  A copy of this object's allocator object
 */
template < int Radix, class Allocator >
inline
typename boost::math::big_radix_whole<Radix, Allocator>::allocator_type
boost::math::big_radix_whole<Radix, Allocator>::get_allocator
(
) const
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    return this->digits_.get_allocator();
}

/** Gets the maximum digit allocations possible for this bignum object.  If the
    current digit allocation count is not significantly smaller than this
    maximum, then performance may suffer due to memory stresses.  Memory limits
    are a concern to operations that result in larger digit strings, like
    additions, multiplications, and up-shifts.

    \return  Let <var>c</var> be a reference to this bignum object's internal
             container.  Return <code><var>c</var>.max_size()</code>.

    \note  Some standard library implementations will make this function return
           <code>std::numeric_limits<size_type>::max()</code> blindly, without
           inspecting the allocator's policy on this point.  So the actual
           memory limit may be smaller.
 */
template < int Radix, class Allocator >
inline
typename boost::math::big_radix_whole<Radix, Allocator>::size_type
boost::math::big_radix_whole<Radix, Allocator>::digit_limit
(
) const
{
    BOOST_PRIVATE_WILD_ASSERT( this->test_invariant() );

    return this->digits_.max_size();
}


#endif  // BOOST_MATH_BIG_RADIX_WHOLE_IMPL_ACCESSOR_IPP

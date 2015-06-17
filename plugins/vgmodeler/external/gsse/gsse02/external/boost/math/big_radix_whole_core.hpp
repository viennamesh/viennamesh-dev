//  Boost math/big_radix_whole_core.hpp header file  -------------------------//

//  Copyright 2006 Daryle Walker.  Distributed under the Boost Software License,
//  Version 1.0.  (See the accompanying file LICENSE_1_0.txt or a copy at
//  <http://www.boost.org/LICENSE_1_0.txt>.)

//  See <http://www.boost.org/libs/math/> for the library's home page.

/** \file
    \brief  An arbitrary-length specific-radix unsigned integer class template

    Contains the definition of the \c boost::math::big_radix_whole\<\> class
    template, which represents nonnegative integers with an arbitrary number of
    digits for a specified radix.  There are arithmetic operators, condition
    functions, and digit access (member) functions.
 */

#ifndef BOOST_MATH_BIG_RADIX_WHOLE_CORE_HPP
#define BOOST_MATH_BIG_RADIX_WHOLE_CORE_HPP

#include <boost/math/big_radix_whole_exceptions.hpp>

#include <climits>  // for INT_MIN, INT_MAX
#include <deque>    // for std::deque
#include <limits>   // for std::numeric_limits
#include <memory>   // for std::allocator
#include <string>   // for std::string
#include <utility>  // for std::pair

// [RH] patch.. 
#include<vector>

#include <boost/cstdint.hpp>              // for boost::uintmax_t
#include <boost/operators.hpp>            // for boost::totally_ordered1, etc.
#include <boost/type_traits/is_same.hpp>  // for boost::is_same
#include <boost/static_assert.hpp>        // for BOOST_STATIC_ASSERT


namespace boost
{
namespace math
{


//  Forward declarations  ----------------------------------------------------//

// The main type
template < int Radix, class Allocator = ::std::allocator<int> >
    class big_radix_whole;

// Operators for the main type
template < int Radix, class Allocator >
    bool  operator !( big_radix_whole<Radix, Allocator> const &x );

template < int Radix, class Allocator >
    big_radix_whole<Radix, Allocator>  operator +( big_radix_whole<Radix,
     Allocator> const &x );
template < int Radix, class Allocator >
    big_radix_whole<Radix, Allocator>  operator -( big_radix_whole<Radix,
     Allocator> const &x );

// Other operations for the main type
template < int Radix, class Allocator >
    void  swap( big_radix_whole<Radix, Allocator> &a, big_radix_whole<Radix,
     Allocator> &b );

template < int Radix, class Allocator >
    big_radix_whole<Radix, Allocator>  abs( big_radix_whole<Radix, Allocator>
     const &x );
template < int Radix, class Allocator >
    int  sgn( big_radix_whole<Radix, Allocator> const &x );

template < int Radix, class Allocator >
    big_radix_whole<Radix, Allocator>  digit_sum( big_radix_whole<Radix,
     Allocator> const &x );


//  Radix-specific bignum unsigned integer class template declaration  -------//

/** \brief  Represents arbitrary-length specific-radix unsigned integers

    Represents a nonnegative integer as a list of digits for a specified radix,
    where the maximum number of digits allowed is limited only by memory.

    \pre  <code><var>Radix</var> &gt;= 2
          && -<var>Radix</var> &gt;= INT_MIN
          && <var>Radix</var><sup>2</sup> &lt;= INT_MAX</code>
    \pre  <code>typename <var>Allocator</var>::&nbsp;value_type</code> is the
          same as <code>int</code>.

    \param Radix      The value of the radix used in the place-value digit
                      string representation.  The value must be workable with
                      conventional place-value arithmetic and without any
                      intermediate result of single-digit addition, subtraction,
                      or multiplication going past the bounds of the
                      <code>int</code> type.  (The given preconditions imply the
                      <code>2 * <var>Radix</var> <= INT_MAX</code> requirement
                      for addition.)
    \param Allocator  The type used for allocating and deallocating any memory
                      required.  It must meet the C++ (2003) standard for such
                      allocator types described in section 20.1.5
                      [lib.allocator.requirements].  If not given,
                      \c std::allocator\<int\> is the default.

    \invariant  The digit string contains integers from <code>0</code> to
                <code><var>Radix</var> - 1</code>.  There are no leading zeros;
                the minimal amount of digits are used.  (This implies that the
                value of zero is represented by the empty digit string.)

    \note  The Boost.Operators library is used to synthesize the following
           operators: <code>!=</code>, <code>++</code> (post), <code>--</code>
           (post), <code>&gt;</code>, <code>&lt;=</code>, <code>&gt;=</code>,
           <code>&gt;&gt;</code>, <code>&lt;&lt;</code>, <code>+</code>
           (binary), <code>-</code> (binary), <code>*</code> (binary),
           <code>/</code>, and <code>%</code>.
 */
template < int Radix, class Allocator >
class big_radix_whole
    : ordered_euclidian_ring_operators1< big_radix_whole<Radix, Allocator>,
        unit_steppable< big_radix_whole<Radix, Allocator>,
        shiftable2< big_radix_whole<Radix, Allocator>, typename std::deque<int,
        Allocator>::size_type > > >
{
    // Pre-conditions
    BOOST_STATIC_ASSERT( Radix >= 2 );
    BOOST_STATIC_ASSERT( Radix + INT_MIN <= 0 );
    BOOST_STATIC_ASSERT( INT_MAX / Radix >= Radix );

    BOOST_STATIC_ASSERT((is_same<int, typename Allocator::value_type>::value));

    // Helper types
    typedef big_radix_whole<Radix, Allocator>  self_type;

// [RH] removed bool_type conversion operator
// [TODO] check this 
    struct dummy { dummy *d; };
    typedef dummy * dummy::*  bool_type;
// ==========

    typedef std::deque<int, Allocator>  deque_type;

    typedef typename deque_type::reference                            reference;
    typedef typename deque_type::const_reference                const_reference;
    typedef typename deque_type::iterator                              iterator;
    typedef typename deque_type::const_iterator                  const_iterator;
    typedef typename deque_type::reverse_iterator              reverse_iterator;
    typedef typename deque_type::const_reverse_iterator  const_reverse_iterator;
    typedef typename deque_type::difference_type                difference_type;

public:
    // Template parameters
    //! Value of the radix
    static  int const  radix = Radix;

    /** \brief  Type of the allocator

        Represents the class used for making memory (de)allocations.  This alias
        exposes the type for later meta-programming.
     */
    typedef Allocator  allocator_type;

    // Other types
    /** \brief  Type for counting digit amounts

        Represents both the count of digits used and an index to reference a
        particular digit.
     */
    typedef typename deque_type::size_type    size_type;
    /** \brief  Type used for digits

        Represents digit values and is used for the internal computations.  It
        should be the <code>int</code> type.
     */
    typedef typename deque_type::value_type  digit_type;

    // Lifetime management (use automatic destructor and copy constructor)
    //! Default and conversion (from built-in integers) constructor
    big_radix_whole( uintmax_t value = 0u, allocator_type const &allocator
     = allocator_type() );

    //! Digit-string constructor
    template < typename InputIterator >
    big_radix_whole( InputIterator begin, InputIterator end, allocator_type
     const &allocator = allocator_type() );

    //! Digits-in-C-string constructor
    explicit  big_radix_whole( std::string const &digit_text, allocator_type
     const &allocator = allocator_type() );

    //! Cross-version copy constructor
    template < int Radix2, class Allocator2 >
    big_radix_whole( big_radix_whole<Radix2, Allocator2> const &source,
     allocator_type const &allocator = allocator_type() );

    /*! \name Accessors */ //@{
    // Accessors
    //! Returns the (minimal) number of digits needed
    size_type  digit_count() const;

    //! Returns the value of a particular digit, quickly
    digit_type  quick_digit_at( size_type index ) const;
    //! Returns the value of a particular digit
    digit_type        digit_at( size_type index ) const;

    //! Returns a copy of the memory allocator used
    allocator_type  get_allocator() const;
    //! Returns the maximum number of digits that can be used
    size_type       digit_limit() const;//@}

    /*! \name Assignment */ //@{
    // Assignment
    //! Exchanges state with another object
    void  swap( big_radix_whole &other );

    //! Changes the current value to zero
    void  reset();
    //! Changes the current value to a new single-digit one
    void  assign_single( digit_type value );

    //! Changes the current value to a new one (built-in unsigned integer)
    void  assign( uintmax_t value );
    //! Changes the current value to a new one (same type)
    void  assign( big_radix_whole const &value );

    //! Changes the current value to a new one (cross-type)
    template < int Radix2, class Allocator2 >
    void  assign( big_radix_whole<Radix2, Allocator2> const &value );

    //! Changes the current value to one based on a digit list
    template < typename InputIterator >
    void  configure( InputIterator begin, InputIterator end );

    //! Changes the current value to one based on a digit list string
    void  configure( char const *digit_text );//@}

    /*! \name Self-Applied Variants of the Unary Operations */ //@{
    // Apply-to-self versions of the unary operations
    //! Assigns Boolean-conversion to self
    void  verify_self();
    //! Assigns Boolean-not to self
    void  invert_self();
    //! "Assigns" same value to self
    void  identify_self() const;
    //! "Assigns" additive inverse to self
    void  negate_self() const;//@}

    // Special multiplication and division operations
    /*! \name Full-Length Add/Subtract-to-Self Fused-Multiply */ //@{
    //! Add the shifted product of two values (fused)
    void  add_shifted_full_product( big_radix_whole const &addend_multiplicand,
     big_radix_whole const &addend_multiplier, size_type index );
    //! Add the product of two values (fused)
    void  add_full_product( big_radix_whole const &addend_multiplicand,
     big_radix_whole const &addend_multiplier );
    //! Subtract the shifted product of two values (fused)
    void  subtract_shifted_full_product( big_radix_whole const
     &subtrahend_multiplicand, big_radix_whole const &subtrahend_multiplier,
     size_type index );
    //! Subtract the product of two values (fused)
    void  subtract_full_product( big_radix_whole const &subtrahend_multiplicand,
     big_radix_whole const &subtrahend_multiplier );
    //! Subtract the shifted product of two values (fused), ignoring sign
    bool  subtract_shifted_full_product_absolutely( big_radix_whole const
     &subtrahend_multiplicand, big_radix_whole const &subtrahend_multiplier,
     size_type index );
    //! Subtract the product of two values (fused), ignoring difference's sign
    bool  subtract_full_product_absolutely( big_radix_whole const
     &subtrahend_multiplicand, big_radix_whole const &subtrahend_multiplier );//@}

    /*! \name Full-Length Division */ //@{
    //! Divide self's value by another to a separate quotient & remainder
    std::pair<big_radix_whole, big_radix_whole>  divide_by( big_radix_whole
     const &divisor ) const;//@}

    //big_radix_whole  pseudo_reciprocal() const;

    //void  square_self();
    //void  power_self( unsigned exponent );
    //void  root_self( unsigned index );
    //void  square_root_self();

    /*! \name Add/Subtract-to-Self Mixed-Mode Fused-Multiply */ //@{
    //! Add the product of factors of various types (fused)
    void  add_mixed_product( big_radix_whole const &addend_full_factor,
     digit_type addend_single_factor, size_type index = 0u );
    //! Subtract the product of factors of various types (fused)
    void  subtract_mixed_product( big_radix_whole const &subtrahend_full_factor,
     digit_type subtrahend_single_factor, size_type index = 0u );
    //! Subtract the product of factors of various types (fused), ignoring sign
    bool  subtract_mixed_product_absolutely( big_radix_whole const
     &subtrahend_full_factor, digit_type subtrahend_single_factor, size_type
     index = 0u );//@}

    /*! \name Single-Digit Multiply-to-Self (Fused-Add/Subtract-to-Self) */ //@{
    //! Fuse-multiply/add by two single-digit values
    void  multiply_single_add_single( digit_type augend_multiplier, digit_type
     addend );
    //! Fuse-multiply/subtract by two single-digit values
    void  multiply_single_subtract_single( digit_type minuend_multiplier,
     digit_type subtrahend );
    //! Multiply by a single-digit value
    void  multiply_single( digit_type multiplier );//@}

    /*! \name Single-Digit Division-to-Self, Modulus, and Parity */ //@{
    //! Divide by a single-digit value, with separate remainder
    digit_type  div_and_mod_single( digit_type divisor );
    //! Returns the remainder from division by a single-digit value
    digit_type  modulo_single( digit_type divisor ) const;
    //! Returns the remainder from division by two (i.e. even versus odd)
    int         parity() const;
    //! Returns whether the current value is odd
    bool        is_odd() const;
    //! Returns whether the current value is even
    bool        is_even() const;//@}

    /*! \name Add/Subtract-to-Self Single-Digit Fused-Multiply */ //@{
    //! Add the shifted product of two single-digit values (fused-add/multiply)
    void  add_shifted_single_product( digit_type addend_multiplicand, digit_type
     addend_multiplier, size_type index );
    //! Add the product of two single-digit values (fused-add/multiply)
    void  add_single_product( digit_type addend_multiplicand, digit_type
     addend_multiplier );
    //! Subtract the shifted product of two single-digit values (fused)
    void  subtract_shifted_single_product( digit_type subtrahend_multiplicand,
     digit_type subtrahend_multiplier, size_type index );
    //! Subtract the product of two single-digit values (fused-sub/multiply)
    void  subtract_single_product( digit_type subtrahend_multiplicand,
     digit_type subtrahend_multiplier );
    //! Subtract shifted product of single-digits (fused), ignoring sign
    bool  subtract_shifted_single_product_absolutely( digit_type
     subtrahend_multiplicand, digit_type subtrahend_multiplier, size_type
     index );
    //! Subtract product of single-digits (fused), ignoring difference's sign
    bool  subtract_single_product_absolutely( digit_type
     subtrahend_multiplicand, digit_type subtrahend_multiplier );//@}

    /*! \name Single-Digit Add/Subtract-to-Self */ //@{
    // Special addition and subtraction operations
    //! Add a single-digit value to a particular place
    void  add_shifted_single( digit_type value, size_type index );
    //! Add a single-digit value
    void  add_single( digit_type addend );
    //! Subtract a single-digit value from a particular place
    void  subtract_shifted_single( digit_type value, size_type index );
    //! Subtract a single-digit value
    void  subtract_single( digit_type subtrahend );
    //! Subtract a shifted single-digit value, ignoring the difference's sign
    bool  subtract_shifted_single_absolutely( digit_type value, size_type
     index );
    //! Subtract a single-digit value, ignoring the difference's sign
    bool  subtract_single_absolutely( digit_type subtrahend );//@}

    /*! \name Full-Length Add/Subtract-to-Self */ //@{
    //! Add a shifted value
    void  add_shifted_full( big_radix_whole const &value, size_type index );
    //! Subtract a shifted value
    void  subtract_shifted_full( big_radix_whole const &value, size_type
     index );
    //! Subtract a shifted value, ignoring the difference's sign
    bool  subtract_shifted_full_absolutely( big_radix_whole const &value,
     size_type index );
    //! Subtract another value, ignoring the difference's sign
    bool  subtract_full_absolutely( big_radix_whole const &subtrahend );//@}

    /*! \name Single Digit Shifting-to/from-Self */ //@{
    // Special shifting operations
    //! Up-shift in a new one's digit
    void        shift_up_add_single( digit_type value );
    //! Down-shift out the current one's digit
    digit_type  mod_radix_shift_down();//@}

    /*! \name Full Comparision from Self */ //@{
    // Special comparison operations
    //! Compares with another object
    int  compare( big_radix_whole const &r ) const;//@}

    /*! \name Self-Applied Variants of the Special Functions */ //@{
    // Apply-to-self versions of the special functions
    //! "Assigns" absolute value to self
    void  absolve_self() const;
    //! Assigns (arithmetic) sign to self
    void  sign_self();//@}

    /*! \name Conversions to Alternate Representations */ //@{
    // Explicit conversions
    //! Converts stored value to a built-in integer
    uintmax_t    to_uintmax() const;
    //! Converts stored value to a built-in float
    double       to_double() const;
    //! Expresses stored value as a string
    std::string  to_string() const;

    //! Converts stored value to a given numeric type
    template < typename NumericType >
    NumericType  to_number() const;//@}

    /*! \name Special Digit Manipulations */ //@{
    // Digit plays
    //! Returns the number of occurrences of a specific digit value
    size_type  specific_digit_count( digit_type value ) const;

    //! Assign digit sum to self
    void        digit_summate_self();
    //! Returns the final result of iterated digit sums
    digit_type  digital_root() const;

    //! Reverse the order of contained digits
    void  reverse_digits();//@}

    /*! \name Operators */ //@{
    // Operators (use automatic copy-assignment operator)
    //! Boolean conversion
    operator bool_type() const;

    //! Pre-increment
    big_radix_whole &  operator ++();
    //! Pre-decrement
    big_radix_whole &  operator --();

    //! Less-than
    bool  operator  <( big_radix_whole const &rhs ) const;
    //! Equals
    bool  operator ==( big_radix_whole const &rhs ) const;

    //! Multiply
    big_radix_whole &  operator *=( big_radix_whole const &multiplier );
    //! Divide
    big_radix_whole &  operator /=( big_radix_whole const &divisor );
    //! Modulo
    big_radix_whole &  operator %=( big_radix_whole const &divisor );
    //! Add
    big_radix_whole &  operator +=( big_radix_whole const &addend );
    //! Subtract
    big_radix_whole &  operator -=( big_radix_whole const &subtrahend );

    //! Left shift
    big_radix_whole &  operator <<=( size_type amount );
    //! Right shift
    big_radix_whole &  operator >>=( size_type amount );//@}

private:
    // State maintainence
    bool  test_invariant() const;

    void  clear_leading_zeros();
    void  normalize();

    // Member data
    deque_type  digits_;

};  // boost::math::big_radix_whole


}  // namespace math
}  // namespace boost


//! \cond
//  Implementation details  --------------------------------------------------//

namespace boost
{
namespace math
{
namespace detail
{

// Nothing here right now.

}  // namespace detail
}  // namespace math
}  // namespace boost
//! \endcond


#include <boost/assert.hpp>  // for BOOST_ASSERT


// Conditionally allow function-start invariant checks
#ifdef BOOST_CONTROL_WILD_ASSERT
#define BOOST_PRIVATE_WILD_ASSERT( Expr )  BOOST_ASSERT( Expr )
#else
#define BOOST_PRIVATE_WILD_ASSERT( Expr )  ((void) 0)
#endif
/** \def  BOOST_PRIVATE_WILD_ASSERT(Expr)

    \brief  Paranoid member-function entry invariant check

    \param Expr  A Boolean expression representing an invariant, the expression
                 should be \c true only if the invariant passes.

    Following the testing strategies in Chapter 10 of the <em>C++ FAQs</em>
    (2<sup>nd</sup> edition) by Marshall Cline <i>et al.</i>, \c BOOST_ASSERT is
    called at the end of every constructor and mutator member function to ensure
    that the class invariant holds.  \c BOOST_PRIVATE_WILD_ASSERT is used for
    the same check at the start of every non-constructor member function.  This
    is usually not needed because mutator-end checking would provide a firewall,
    but the method-start checks ensure safety from a wild pointer ruining an
    object between method calls.  Since the method-start check is usually
    unnecessary, \c BOOST_PRIVATE_WILD_ASSERT is \#defined to do nothing.  If
    the control macro \c BOOST_CONTROL_WILD_ASSERT gets \#defined, then
    \c BOOST_PRIVATE_WILD_ASSERT will act like the \c BOOST_ASSERT macro.
 */


//  Radix/bignum/natural static member data definitions  ---------------------//

/** Exposes the radix of the arithmetic computations for later meta-programming.
    Even though it's a compile-time constant, it has a separate definition to
    satisfy some compilers.
 */
template < int Radix, class Allocator >
int const  boost::math::big_radix_whole<Radix, Allocator>::radix;


//  Radix/bignum/natural separate definition #includes  ----------------------//

#include <boost/math/impl/big_radix_whole_internal.ipp>
#include <boost/math/impl/big_radix_whole_accessor.ipp>
#include <boost/math/impl/big_radix_whole_constructor.ipp>
#include <boost/math/impl/big_radix_whole_assign.ipp>
#include <boost/math/impl/big_radix_whole_compare.ipp>
#include <boost/math/impl/big_radix_whole_shifting.ipp>
#include <boost/math/impl/big_radix_whole_addition.ipp>
#include <boost/math/impl/big_radix_whole_subtraction.ipp>
#include <boost/math/impl/big_radix_whole_multiplication.ipp>
#include <boost/math/impl/big_radix_whole_division.ipp>
#include <boost/math/impl/big_radix_whole_conversion.ipp>
#include <boost/math/impl/big_radix_whole_math.ipp>


//  Radix/bignum/natural extra member function definitions  ------------------//

// Put something here?


//  Radix/bignum/natural operator member function definitions  ---------------//

// Put something here?


//  Radix/bignum/natural miscellaneous function definitions  -----------------//

// Put something here?


//  Radix/bignum/natural Numeric Limits definition  --------------------------//

namespace std
{

/** \brief  \c std::numeric_limits\<\> (partial) specialization for
            \c boost::math::big_radix_whole\<\>

    Since \c boost::math::big_radix_whole\<\> models a subset of the real
    numbers, it can get a specialization of the Standard numeric-limits class
    template.  Only the members appropriate for an unlimited-length and
    non-negative integer type are meaningfully defined.
 */
template < int Radix, class Allocator >
class numeric_limits< ::boost::math::big_radix_whole<Radix, Allocator> >
{
    typedef ::boost::math::big_radix_whole<Radix, Allocator>  source_type;

public:
    //! Indicates if a particular specialization was made
    static  bool const  is_specialized = true;

    // These are the entries meaningful for unlimited-length non-negative
    // integer types.
    //! Minimum finite value
    static  source_type  min() throw()   { return source_type(); }

    //! Indicates if the source type is signed
    static  bool const  is_signed  = false;
    //! Indicates if the source type is integral
    static  bool const  is_integer = true;
    //! Indicates if the source type is exact (i.e. not floating)
    static  bool const  is_exact   = true;
    //! Base of the representation
    static  int const   radix      = Radix;

    //! Indicates if the source type conforms to IEC 559
    static  bool const  is_iec559  = false;
    //! Indicates if the source type represents a confined set of values
    static  bool const  is_bounded = false;
    //! Indicates if arithmetic operations can wrap around
    static  bool const  is_modulo  = false;

    //! Indicates if the source type implements trapping
    static  bool const  traps = false;

    // These entries are only for floating-point or otherwise non-matching
    // types.  The function entries are never defined.
    //! Maximum finite value
    static  source_type  max() throw();

    //! Number of \c radix digits that can be represented without change
    static  int const  digits   = 0;
    //! Number of decimal digits that can be represented without change
    static  int const  digits10 = 0;

    //! \p x - 1, where \p x is the least representable value greater than 1
    static  source_type  epsilon() throw();
    //! Measure of the maximum rounding error
    static  source_type  round_error() throw();

    //! Smallest negative \p x such that \p radix**(x-1) is still normalized
    static  int const  min_exponent   = 0;
    //! Smallest negative \p x such that 10**<var>x</var> is still normalized
    static  int const  min_exponent10 = 0;
    //! Largest positive \p x such that \p radix**(x-1) is still regular
    static  int const  max_exponent   = 0;
    //! Largest positive \p x such that 10**<var>x</var> is still regular
    static  int const  max_exponent10 = 0;

    //! Indicates if the source type can represent positive infinity
    static  bool const                has_infinity      = false;
    //! Indicates if the source type can represent non-signaling not-a-numbers
    static  bool const                has_quiet_NaN     = false;
    //! Indicates if the source type can represent a signaling not-a-number
    static  bool const                has_signaling_NaN = false;
    //! Indicates if the source type could represent denormalized values
    static  float_denorm_style const  has_denorm        = denorm_absent;
    //! Indicates if inaccuracies cause either denormalized or inexact results
    static  bool const                has_denorm_loss   = false;

    //! Returns a representation of positive infinity, if possible
    static  source_type  infinity() throw();
    //! Returns a representation of a quiet not-a-number, if available
    static  source_type  quiet_NaN() throw();
    //! Returns a representation of a signaling not-a-number, if available
    static  source_type  signaling_NaN() throw();
    //! Returns the minimum positive (de)normalized value
    static  source_type  denorm_min() throw();

    //! Indicates if the source type detects tinyness before rounding
    static  bool const               tinyness_before = false;
    //! Indicates how the source type performs rounding
    static  float_round_style const  round_style     = round_toward_zero;
}; 

}  // namespace std


// Remove any private macros
#undef BOOST_PRIVATE_WILD_ASSERT


#endif  // BOOST_MATH_BIG_RADIX_WHOLE_CORE_HPP

/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright 2006 Daryle Walker.  

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */
//
//  Revision History
//   23-11-2006  Initial version (Daryle Walker)
//   10-08-2007  integration into gsse
//   13-03-2008  update for gcc 4.3 / more integration into gsse as numerical data type
// 

#include "gsse/external/boost/math/big_radix_whole.hpp"  // for boost::math::big_radix_whole

namespace gsse {

// Types & templates
using boost::math::big_radix_whole;


// =======================================================================
//

// Find the best square-root approximation for INT_MAX
class sqrt_int_max_finder
{
    template < int DigitAmount, bool DigitAmountEven = true >
    struct finder
    {
        static  int const  sqrt_max = 1 << DigitAmount / 2;
    };

    template < int DigitAmount >
    struct finder<DigitAmount, false>
    {
        // I think/hope that built-in integers MUST be base-2.
        // 70/99 is an approximation for 1/sqrt(2)
        static  int const  sqrt_max = (1 << (DigitAmount + 1) / 2) * 70 / 99;
    };

    typedef std::numeric_limits<int>  limits_type;

public:
    static  int const  sqrt_max = finder<limits_type::digits,
     (limits_type::digits % 2 == 0)>::sqrt_max;

};

template < int DigitAmount, bool DigitAmountEven >
int const  sqrt_int_max_finder::finder<DigitAmount, DigitAmountEven>::sqrt_max;

template < int DigitAmount >
int const  sqrt_int_max_finder::finder<DigitAmount, false>::sqrt_max;

int const  sqrt_int_max_finder::sqrt_max;



// =======================================================================
// Helper functions
//

// Get the beginning iterator for a built-in array
template < typename T, std::size_t N >
inline  T *  array_begin( T (&a)[N] )  { return &a[0]; }

template < typename T, std::size_t N >
inline  T const *  array_begin( T const (&a)[N] )  { return &a[0]; }

// Get the ending iterator for a built-in array
template < typename T, std::size_t N >
inline  T *  array_end( T (&a)[N] )  { return &a[N]; }

template < typename T, std::size_t N >
inline  T const *  array_end( T const (&a)[N] )  { return &a[N]; }

// Create parity maps
std::vector<int>  parity_list( std::size_t max_value )
{
    std::vector<int>  result;

    result.reserve( max_value + 1u );
    for ( std::size_t  i = 0 ; i <= max_value ; ++i )
    {
        result.push_back( i & 1u );
    }
    return result;
}

// Create a flipped copy of a vector
std::vector<bool>  flip_vector( std::vector<bool> const &v )
{  std::vector<bool>  result( v );  result.flip();  return result;  }





typedef big_radix_whole<sqrt_int_max_finder::sqrt_max>  big_ultimate;

} // namespace gsse

/* ============================================================================
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at
   Copyright (c) 2004-2010 Philipp Schwaha                      philipp@gsse.at
   Copyright (c) 2009-2010 Josef Weinbub                          josef@gsse.at   
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#ifndef GSSE_GENERICDELAUNAY_HH_ID
#define GSSE_GENERICDELAUNAY_HH_ID 

// *** system includes
// *** BOOST includes
// *** GSSE includes
#include "gsse/geometry/predicates_shewchuk_wrapper.hpp"
#include "gsse/numerics/conversion.hpp"

namespace gsse
{

// 
// DELAUNAY TEST based on JR Shewchuk
// 
// returns: true for delaunay conformity
// returns: false for non-delaunay conformity
//
template < int TDIM, typename Entity = gsse::cell_simplex > 
struct delaunay_test_shew_impl 
{ 
   template<typename Cell, typename Point, typename ErrorStream>
   static bool   
   eval( Cell        const& cell, 
         Point       const& pt_test, 
         ErrorStream      & error_stream, 
         double      const& comparison_value_double)
   {
      error_stream << "### ERROR ### delaunay test shewchuk not implemented for: " << std::endl;
      error_stream << "   TDIM: " << TDIM << std::endl;
      gsse::dump_type< Entity >();  
      return false;
   }
};

template < >
struct delaunay_test_shew_impl < 4, gsse::cell_simplex >  // 4 points = 3-simplex
{
   template<typename Cell, typename Point, typename ErrorStream>
   static bool
   eval( Cell        & cell, 
         Point       & pt_test, 
         ErrorStream      & error_stream, 
         double       comparison_value_double)
   {
      
//       gsse::dump_type(cell[0]);
//       gsse::dump_type(pt_test);
//       exit(0);
      
//      short result = gsse::in_n_sphere<gsse::predicate::adaptive>()(cell[0], cell[1], cell[2], cell[3], pt_test);
      short result = gsse::in_n_sphere<gsse::predicate::adaptive>()(cell, pt_test);
   
      // if outside ..
      if(result == -1)
      {
         // .. the cell is Delaunay
         return true;
      }
      // if inside ..
      else
      if(result == 1)
      {
         // .. the cell is NOT Delaunay
         return false;
      }
      // if the point is on the sphere ..
      else
      {
         // .. the cell MIGHT be non-Delaunay
         return true;
      }
   }
};

// 
// returns: true for delaunay conformity
// returns: false for non-delaunay conformity
//
struct delaunay_test_shew
{
   template<typename Cell, typename Point, typename ErrorStream>
   bool operator()(  Cell        & cell, 
                     Point       & pt_test, 
                     ErrorStream      & error_stream = std::cerr, 
                     double       comparison_value_double = -1e-2)
   {
      // [JW] TODO we need a generic size retrieval, like in the gml
      // atm it is specialized on boost::arrays, which is the datatype 
      // which holds gsse cells
      //
      return delaunay_test_shew_impl< Cell::static_size >::eval(
         cell, pt_test, error_stream, comparison_value_double);
   }   
};
// ==================================================================================


// [JW]TODO where to check for the entity?! 
// however, it should be solely present on the evaluation level, 
// wo we can apply the same functor on different entities ..
// 
template < int TDIM, typename Entity = gsse::cell_simplex > 
struct delaunay_test_impl 
{ 
   template<typename Cell, typename Point, typename ErrorStream>
   static bool   
   eval( Cell        const& cell, 
         Point       const& pt_test, 
         ErrorStream      & error_stream, 
         double      const& comparison_value_double)
   {
      error_stream << "### ERROR ### delaunay test not implemented for: " << std::endl;
      error_stream << "   TDIM: " << TDIM << std::endl;
      gsse::dump_type< Entity >();  
      return false;
   }
};


template < >
struct delaunay_test_impl < 4, gsse::cell_simplex >  // 4 points = 3-simplex
{
   template<typename Cell, typename Point, typename ErrorStream>
   static bool
   eval( Cell        const& cell, 
         Point       const& pt_test, 
         ErrorStream      & error_stream, 
         double      const& comparison_value_double)
   {
      bool is_delaunay = false;

      Point pt_cc     = gsse::circumcenter_simplex( cell );

      Point vec_cc      = cell[0] - pt_cc;
      Point vec_cc_test = pt_test - pt_cc;

      typename Point::value_type length_delaunay_offset =   gsse::length_square (vec_cc_test) - gsse::length_square (vec_cc);

      typename Point::value_type  comparison_value;
//      gsse::numerical_conversion_simple()(comparison_value, comparison_value_double);
      gsse::numerical_conversion()(comparison_value_double, comparison_value );

      //   std::cout << "###### delaunay .. " << length_delaunay_offset << std::endl;
      //   std::cout << "####   cmp value: " << comparison_value << std::endl;
      //   std::cout << "### " << std::endl;

      if ( length_delaunay_offset <  comparison_value)
      {
         error_stream << " ## difference in length: " << length_delaunay_offset << std::endl;
         
         error_stream << "...  pt: " << pt_test << std::endl;
         error_stream << "...  cc: " << pt_cc << std::endl;
         
         error_stream << "...  len cc:   " << gsse::length_square(vec_cc) << std::endl;
         error_stream << "...  len ccpt: " << gsse::length_square(vec_cc_test) << std::endl;
      }
      else
      {
//       std::cout << "## delaunay conform .. " << std::endl;
         is_delaunay = true;
      }

      return is_delaunay;
   }
};

// 
// DELAUNAY TEST based on RH
// 
// returns: true for delaunay conformity
// returns: false for non-delaunay conformity
//
struct delaunay_test
{
   template<typename Cell, typename Point, typename ErrorStream>
   bool operator()(  Cell        const& cell, 
                     Point       const& pt_test, 
                     ErrorStream      & error_stream = std::cerr, 
                     double      const& comparison_value_double = -1e-2)
   {
      // [JW] TODO we need a generic size retrieval, like in the gml
      // atm it is specialized on boost::arrays, which is the datatype 
      // which holds gsse cells
      //
      return delaunay_test_impl< Cell::static_size >::eval(
         cell, pt_test, error_stream, comparison_value_double);
   }   
};



} // namespace gsse

#endif

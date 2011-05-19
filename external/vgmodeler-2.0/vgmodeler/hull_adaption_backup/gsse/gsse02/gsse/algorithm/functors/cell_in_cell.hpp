/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GML_GEOMETRY_FUNCTORS_CELLINCELL
#define GML_GEOMETRY_FUNCTORS_CELLINCELL
//
// ===================================================================================
//
// *** SYSTEM includes
//
#include <vector>  
//
// *** GML includes
//
#include "../../util/common.hpp"
#include "../../util/tags.hpp"
#include "../../functors/generic_functor_binary.hpp"
#include "../algorithms/predicates.hpp"
#include "cell_orientation.hpp"
//
// *** GSSE includes
//
#include <gsse/geometry/metric_operations.hpp>
#include <gsse/datastructure/container.hpp>
//
// *** BOOST includes
//
#include <boost/type_traits/is_same.hpp>
//
// ===================================================================================
//
//#define DEBUG_CELLINCELL
//
// ===================================================================================
//
namespace gml {

//
// ===================================================================================
//

namespace result_of {

// [JW] TODO resolve the gsse/gml - tag ambiguities
template < long     DIMResult >
struct cell_in_cell
{
   typedef typename gsse::topology::result_of::cell_calc_container<DIMResult, gsse::cell_simplex>::type  type;
};

} // end namespace: result_of
//
// ===================================================================================
//
namespace detail {
template < int DIMTResult1, int DIMTResult2, typename CellTopology >
struct cell_in_cell_impl
{
   // ------------------------------------------
   typedef boost::phoenix::function< cell_in_cell_impl< 0, DIMTResult2, CellTopology > > PointInCell;
   // ------------------------------------------
   template < typename Element1, typename Element2 >
   struct result
   { 
      typedef int           type;
   };
   // ------------------------------------------
   template < typename Element1, typename Element2 >
   inline 
   typename result< Element1, Element2 >::type
   operator()(Element1& ele1, Element2& ele2) const
   {
   #ifdef DEBUG_CELLINCELL
      std::cout << "CELL-IN-CELL::DEFAULT" << std::endl;
      std::cout << "  DIMTResult1: " << DIMTResult1 
                << " -- DIMTResult2: " << DIMTResult2 << std::endl;
      std::cout << "  ele1: " << ele1 << std::endl;
      std::cout << "  ele2: " << ele2 << std::endl;
   #endif
      namespace phoenix = boost::phoenix; 
      using namespace phoenix::arg_names;
      
      gsse::array< typename gml::result_of::val< Element1 >::type, 1 >  ele1_value_cell;
      
      int ret_val = 1;
      
      gsse::traverse()
      [
         phoenix::at( phoenix::ref(ele1_value_cell), 0 ) = _1,
         phoenix::if_( point_in_cell( phoenix::ref(ele1_value_cell), phoenix::ref(ele2) ) == -1 )
         [
            phoenix::ref(ret_val) = -1
         ]
      ](ele1);

      return ret_val;
   }
   // ------------------------------------------
   PointInCell point_in_cell;
   // ------------------------------------------
};
//
// 0-CELL IN 0-CELL 
// 
//    aka: point in/on point 
//    aka: colocal
//    returns: 
//              1 .. colocal           
//              -1 .. not-colocal  
//
template < typename CellTopology >
struct cell_in_cell_impl < 0, 0, CellTopology >
{
   // ------------------------------------------
   template < typename Element1, typename Element2 >
   struct result
   { 
      typedef int           type;
   };
   // ------------------------------------------
   template < typename Element1, typename Element2 >
   inline 
   typename result< Element1, Element2 >::type
   operator()(Element1& ele1, Element2& ele2) const
   {
   #ifdef DEBUG_CELLINCELL
      std::cout << "CELL-IN-CELL:: < 0, 0, Cell > " << std::endl;
      std::cout << "  ele1: " << ele1 << std::endl;
      std::cout << "  ele2: " << ele2 << std::endl;
   #endif
      if( gsse::length( ele1[0] - ele2[0] ) > 1.0E-15 ) return -1;
      else return 1;
   }
};
//
// 0-CELL IN 1-CELL 
// 
//    aka:   point in line
//    brief: determines wheter or not the point is on 
//           the finite line determined by two points
//
template < typename CellTopology >
struct cell_in_cell_impl < 0, 1, CellTopology >
{
   // ------------------------------------------
   template < typename Element1, typename Element2 >
   struct result
   { 
      typedef int           type;
   };
   // ------------------------------------------
   template < typename Element1, typename Element2 >
   inline 
   typename result< Element1, Element2 >::type
   operator()(Element1& ele1, Element2& ele2) const
   {
   #ifdef DEBUG_CELLINCELL
      std::cout << "CELL-IN-CELL:: < 0, 1, Cell > " << std::endl;
      std::cout << "  ele1: " << ele1 << std::endl;
      std::cout << "  ele2: " << ele2 << std::endl;
   #endif   
   
      return same_type( ele1, ele2, 
         boost::is_same< 
            typename gml::result_of::val< Element1 >::type, 
            typename gml::result_of::val< Element2 >::type 
         >() 
      );
   }
   
   template < typename Element1, typename Element2 >
   inline 
   typename result< Element1, Element2 >::type
   same_type(Element1& ele1, Element2& ele2, boost::true_type const&) const
   {
      gsse::array< typename gml::result_of::val< Element1 >::type, 3 >  combined_ele;

      combined_ele[0] = ele2[0];
      combined_ele[1] = ele2[1];      
      combined_ele[2] = ele1[0];
   
      // test if the point ( of ele1 ) is on the infinite line defined by two points
      // of ele2 
      //
      if( gml::detail::cell_orientation_impl< 2, gml::cell_simplex >()( combined_ele ) == 0 )
      {
      #ifdef DEBUG_CELLINCELL
         std::cout << " its on the infinite line .. " << std::endl;
      #endif
         // if the point is additionally within the box defined by the two points of ele2,
         // the point ( ele1 ) is on the finite line between the points of ele2.
         //
         return gml::algorithms::point_in_box( ele2[0], ele2[1], ele1[0] );
      }
      return -1;
   }
   
   template < typename Element1, typename Element2 >
   inline 
   typename result< Element1, Element2 >::type
   same_type(Element1& ele1, Element2& ele2, boost::false_type const&) const
   {
      std::cout << "ERROR: CELL-IN-CELL-IMPL::DIFFERENT ELEMENT HANDLING NOT IMPLEMENTED!" << std::endl;
      return 0;
   }   
   
};
//
// 0-CELL IN 2-CELL 
// 
//    aka:   point in triangle
//    returns:
//              1 .. inside           
//              0 .. on  
//             -1 .. outside  
//
template < >
struct cell_in_cell_impl < 0, 2, gml::cell_simplex >
{
   // ------------------------------------------
   template < typename Element1, typename Element2 >
   struct result
   { 
      typedef int           type;
   };
   // ------------------------------------------
   template < typename Element1, typename Element2 >
   inline 
   typename result< Element1, Element2 >::type
   operator()(Element1& ele1, Element2& ele2) const
   {
   #ifdef DEBUG_CELLINCELL
      std::cout << "CELL-IN-CELL:: < 0, 2, Simplex > " << std::endl;
      std::cout << "  ele1: " << ele1 << std::endl;
      std::cout << "  ele2: " << ele2 << std::endl;
   #endif       
      return compiletime_spec( ele1, ele2, 
         gml::is_compiletime< 
            typename gml::traits::tag_of< 
               typename gml::result_of::val< Element2 >::type
            >::evaluation_type
         >() 
      );
   }
   // ------------------------------------------   
   template < typename Element1, typename Element2 >
   inline 
   typename result< Element1, Element2 >::type
   compiletime_spec(Element1& ele1, Element2& ele2, boost::true_type const&) const
   {
      return gml::algorithms::point_in_triangle< 
         gml::result_of::size< 
            typename gml::result_of::val< Element2 >::type
         >::value
      >::compute( ele2[0], ele2[1], ele2[2], ele1[0] );
   }
   // ------------------------------------------        
   template < typename Element1, typename Element2 >
   inline 
   typename result< Element1, Element2 >::type
   compiletime_spec(Element1& ele1, Element2& ele2, boost::false_type const&) const
   {
      long geom_dim = gsse::size(ele2[0]);
      if( geom_dim == 2 ) 
         return gml::algorithms::point_in_triangle< 2 >::compute( ele2[0], ele2[1], ele2[2], ele1[0] );
      if( geom_dim == 3 ) 
         return gml::algorithms::point_in_triangle< 3 >::compute( ele2[0], ele2[1], ele2[2], ele1[0] );
      
      std::cout << "ERROR::CELL-IN-CELL: geometry dimension: " << geom_dim << " is not supported .. " << std::endl;
      return 0.0;
   }   
   // ------------------------------------------   
};
//
// 0-CELL IN 3-CELL 
// 
//    aka:     point in tetrahedron
//    returns:
//              1 .. inside           
//              0 .. on  
//             -1 .. outside  
//
template < >
struct cell_in_cell_impl < 0, 3, gml::cell_simplex >
{
   // ------------------------------------------
   template < typename Element1, typename Element2 >
   struct result
   { 
      typedef int           type;
   };
   // ------------------------------------------
   template < typename Element1, typename Element2 >
   inline 
   typename result< Element1, Element2 >::type
   operator()(Element1& ele1, Element2& ele2) const
   {
   #ifdef DEBUG_CELLINCELL
      std::cout << "CELL-IN-CELL:: < 0, 3, Simplex > " << std::endl;
      std::cout << "  ele1: " << ele1 << std::endl;
      std::cout << "  ele2: " << ele2 << std::endl;
   #endif      
      return gsse::swk_orient3D( ele2[0], ele2[1], ele2[2], ele2[3], ele1[0]);
   }
};
//
// ===================================================================================
//
} // end namespace: detail
//
// ===================================================================================
//
template < typename NonEvaluatedProperty >
struct cell_in_cell
{
   typedef typename NonEvaluatedProperty::type    Property;

   typedef typename gml::generic_functor_binary < 
      boost::phoenix::function< 
         gml::detail::cell_in_cell_impl<
            Property::DIMResult1,
            Property::DIMResult2,
            typename Property::cell_topology
         > 
      >,
      typename gml::result_of::cell_in_cell<
         Property::DIMResult1
      >::type,
      Property
   >::type  type;    
};
//
// ===================================================================================
//

} // end namespace: gml

#endif






















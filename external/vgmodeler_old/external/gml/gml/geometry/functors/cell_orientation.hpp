/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GML_GEOMETRY_FUNCTORS_CELLORIENTATION
#define GML_GEOMETRY_FUNCTORS_CELLORIENTATION
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
#include "../../geometry/algorithms/predicates.hpp"
#include "../../functors/generic_functor_unary.hpp"
#include "../../util/property.hpp"
//
// ===================================================================================
//
//#define DEBUG_METRICQUANTITY
//
// ===================================================================================
//
namespace gml {

//
// ===================================================================================
//
namespace result_of {

template < typename Numeric >
struct cell_orientation
{
   typedef std::vector< Numeric >   type;
};

} // end namespace: result_of
//
// ===================================================================================
//
namespace detail {
template < int DIMTResult, typename CellTopology >
struct cell_orientation_impl
{
};
//
// 2-simplex specialization
//
template < >
struct cell_orientation_impl < 2, gml::cell_simplex >
{
   // ------------------------------------------
   template < typename Element >
   struct result
   { 
      typedef int           type;
   };
   // ------------------------------------------
   template < typename Element >
   inline 
   typename result< Element >::type
   operator()(Element& ele) const
   {
      return compiletime_spec( ele, 
         gml::is_compiletime< 
            typename gml::traits::tag_of< 
               typename gml::result_of::val< Element >::type
            >::evaluation_type
         >() 
      );
   }
   // ------------------------------------------   
   template < typename Element >
   inline 
   typename result< Element >::type
   compiletime_spec(Element& ele, boost::true_type const&) const
   {
      return gml::algorithms::evaluate(
         gml::algorithms::orient2d< 
            gml::result_of::size< 
               typename gml::result_of::val< Element >::type
            >::value
         >::compute( ele[0], ele[1], ele[2] )
      );
   }
   // ------------------------------------------        
   template < typename Element >
   inline 
   typename result< Element >::type
   compiletime_spec(Element& ele, boost::false_type const&) const
   {
      long geom_dim = gsse::size(ele[0]);
      if( geom_dim == 2 ) 
         return gml::algorithms::evaluate( gml::algorithms::orient2d< 2 >::compute( ele[0], ele[1], ele[2] ) );
      if( geom_dim == 3 ) 
         return gml::algorithms::evaluate( gml::algorithms::orient2d< 3 >::compute( ele[0], ele[1], ele[2] ) );
      
      std::cout << "ERROR::ORIENTATION: geometry dimension: " << geom_dim << " is not supported .. " << std::endl;
      return 0.0;
   }   
   // ------------------------------------------   
};
//
// 3-simplex specialization
//
template < >
struct cell_orientation_impl < 3, gml::cell_simplex >
{
   // ------------------------------------------
   template < typename Element >
   struct result
   { 
      typedef typename gml::result_of::val< 
         typename gml::result_of::val<
            Element
         >::type
      >::type           type;
   };
   // ------------------------------------------
   template < typename Element >
   inline 
   typename result< Element >::type
   operator()(Element& ele) const
   {
      return gml::algorithms::evaluate( gml::algorithms::jrs_orient3dfast( ele[0], ele[1], ele[2], ele[3] ) );     
   }
   // ------------------------------------------
};
//
// ===================================================================================
//
} // end namespace: detail
//
// ===================================================================================
//
template < typename NonEvaluatedProperty >
struct cell_orientation
{
   typedef typename NonEvaluatedProperty::type    Property;

   typedef typename gml::generic_functor_unary < 
      boost::phoenix::function< 
         gml::detail::cell_orientation_impl<
            Property::DIMResult,
            typename Property::cell_topology
         > 
      >,
      typename gml::result_of::cell_orientation< int >::type,
      Property
   >::type  type;      
};
//
// ===================================================================================
//

} // end namespace: gml

#endif






















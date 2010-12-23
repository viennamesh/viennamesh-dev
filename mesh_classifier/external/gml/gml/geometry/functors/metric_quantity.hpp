/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GML_GEOMETRY_FUNCTORS_METRICQUANTITY
#define GML_GEOMETRY_FUNCTORS_METRICQUANTITY
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
#include "../../geometry/algorithms/area.hpp"
#include "../../functors/generic_functor_unary.hpp"
#include "../../util/property.hpp"
//
// *** GSSE includes
//
#include <gsse/geometry/metric_operations.hpp>
#include <gsse/algorithm/io.hpp>
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
struct metric_quantity
{
   typedef std::vector< Numeric >   type;
};

} // end namespace: result_of
//
// ===================================================================================
//
namespace detail {
template < int DIMTResult, typename CellTopology >
struct metric_quantity_impl
{
};
//
// length
//
template < typename CellTopology >
struct metric_quantity_impl < 1, CellTopology >
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
      return gsse::length( ele[0]-ele[1] );
   }
   // ------------------------------------------
};
//
// area triangle
//
template < >
struct metric_quantity_impl < 2, gml::cell_simplex >
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
      return metric_quantity_impl< 2, gml::cell_cube >()(ele) / 2.0;
   }
   // ------------------------------------------   
};
//
// area rectangle
//
template < >
struct metric_quantity_impl < 2, gml::cell_cube >
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
      //
      // compile time function evaluation to determine the 
      // best area algorithm. for a geometry dimension of 2 
      // we can use shewchuks orient2dfast algorithm, for 
      // a geometry dimension of 3, we need to use the more general 
      // approach based on the norm of the exproduct vector.
      //
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
      return gml::algorithms::area_parallelogram< 
         gml::result_of::size< 
            typename gml::result_of::val< Element >::type
         >::value
      >::compute( ele[0], ele[1], ele[2] );
   }
   // ------------------------------------------        
   template < typename Element >
   inline 
   typename result< Element >::type
   compiletime_spec(Element& ele, boost::false_type const&) const
   {
      long geom_dim = gsse::size(ele[0]);
      if( geom_dim == 2 ) 
         return gml::algorithms::area_parallelogram< 2 >::compute( ele[0], ele[1], ele[2] );
      if( geom_dim == 3 ) 
         return gml::algorithms::area_parallelogram< 3 >::compute( ele[0], ele[1], ele[2] );
      
      std::cout << "ERROR::METRIC-QUANTITY: geometry dimension: " << geom_dim << " is not supported .. " << std::endl;
      return 0.0;
   }   
   // ------------------------------------------   
};
//
// volume tetrahedron
//
template < >
struct metric_quantity_impl < 3, gml::cell_simplex >
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
      return gml::algorithms::jrs_orient3dfast( ele[0], ele[1], ele[2], ele[3] ) / 6.0;
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
struct metric_quantity
{
   typedef typename NonEvaluatedProperty::type    Property;

   typedef typename gml::generic_functor_unary < 
      boost::phoenix::function< 
         gml::detail::metric_quantity_impl<
            Property::DIMResult,
            typename Property::cell_topology
         > 
      >,
      typename gml::result_of::metric_quantity< double >::type,
      Property
   >::type  type;      
};

//
// ===================================================================================
//

} // end namespace: gml

#endif






















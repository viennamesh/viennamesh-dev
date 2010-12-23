/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GML_FUNCTORS_NEF
#define GML_FUNCTORS_NEF

//
// *** GML includes
//
#include "unary_generic_functor.hpp"
#include "../datastructure/cgal.hpp"
#include "../util/debug.hpp"

namespace gml {
// ===================================================================================
namespace result_of {
// ===================================================================================
//
// ===================================================================================
//
template < int DIMResult >
struct nef
{
   typedef std::vector< 
      typename gml::result_of::cgal::nef_polyhedron< DIMResult >::type
   >   type;
};
//
// ===================================================================================
//
// ===================================================================================
} // end namespace: result_of
// ===================================================================================

// ===================================================================================
namespace detail {
// ===================================================================================
template < int DIMTResult, typename CellTopology >
struct nef_impl { };

template < >
struct nef_impl < 3, gml::cell_simplex >
{
   // ------------------------------------------
   typedef gml::result_of::cgal::nef_polyhedron< 3 >::type  PolyhedronT;
   typedef gml::result_of::cgal::plane< 3 >::type           PlaneT;
   typedef gml::result_of::cgal::point< 3 >::type           PointT;
   // ------------------------------------------
   template < typename Element >
   struct result
   { 
      typedef PolyhedronT           type;
   };
   // ------------------------------------------
   template < typename Element >
   inline 
   typename result< Element >::type
   operator()(Element& ele) const
   {
      PolyhedronT N1( 
         PlaneT( 
            PointT( ele[0][0], 0.0f, ele[0][2] ),
            PointT( 1.0f, 0.0f, 0.0f ),
            PointT( 0.0f, 1.0f, 0.0f )
         )
      );            
   
      gml::dumptype ( 0.0f );
      gml::dumptype ( ele[0][0] );
   
   /*
      PolyhedronT N1( 
         PlaneT( 
            PointT( ele[0][0], ele[0][1], ele[0][2] ),
            PointT( ele[1][0], ele[1][1], ele[1][2] ),
            PointT( ele[2][0], ele[2][1], ele[2][2] )
         )
      );            
    */
      return N1;
   }
   // ------------------------------------------   
};   


// ===================================================================================
} // end namespace: detail
// ===================================================================================


template < typename NonEvaluatedProperty >
struct nef
{
   typedef typename NonEvaluatedProperty::type    Property;

   typedef typename gml::unary_generic_functor < 
      boost::phoenix::function< 
         gml::detail::nef_impl<
            Property::DIMResult,
            typename Property::cell_topology
         > 
      >,
      typename gml::result_of::nef< 
         Property::DIMResult
      >::type,
      Property
   >::type  type;      
};

} // end namespace: gml

#endif

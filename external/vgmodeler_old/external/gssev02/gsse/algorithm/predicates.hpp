/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
#ifndef GML_GEOMETRY_ALGORITHMS_PREDICATES
#define GML_GEOMETRY_ALGORITHMS_PREDICATES
//
// ===================================================================================
//
// *** SYSTEM includes
//
#include <limits>
#include <map>
//
// *** GML includes
//
#include "predicates_jrs.hpp"
#include "../../util/for_each.hpp"
#include "../../functors/fabs.hpp"
//
// *** GSSE includes
//
#include <gsse/geometry/orient.hpp>
//
// *** BOOST includes
//
#include <boost/spirit/home/phoenix/core.hpp>  
#include <boost/spirit/home/phoenix/operator.hpp>  
#include <boost/spirit/home/phoenix/statement.hpp>  
//
// ===================================================================================
//
namespace gml 
{
namespace algorithms
{
//
// ===================================================================================
//
// POINT-IN-BOX TEST
//
//   brief: tests if pnt is inside the box defined by the 
//          rectangle/cuboid. the box is constructed by 
//          interpreting the two points pa, pb as diagonal 
//          subtending points.
//
//   returns:
//          1 .. inside + boundary
//          -1 .. outside
//
template < typename MetricalSpace1, typename MetricalSpace2 >
int point_in_box( MetricalSpace1 const& pa, 
                  MetricalSpace1 const& pb,
                  MetricalSpace2 const& pnt )
{
   namespace phoenix = boost::phoenix; 
   using namespace phoenix::arg_names;
   
   // [JW] TODO add ON-BOUNDARY functionality .. return 0 ..
   
   int in_flag = 1;
   gml::for_each( pa, pb, pnt,
   (
      //std::cout << _1 << phoenix::val(" - ") << _2 << phoenix::val(" - ") << _3 << std::endl,
   
      phoenix::if_( _1 < _2 )
      [
         phoenix::if_( (_3 < _1) || (_3 > _2) )
         [
            phoenix::ref(in_flag) = -1
         ]
      ]
      .else_
      [
         phoenix::if_( (_3 < _2) || (_3 > _1) )
         [
            phoenix::ref(in_flag) = -1
         ]         
      ]
   ));
   return in_flag;
}
//
// ===================================================================================
//
template < typename Numeric >
int
evaluate ( Numeric const& val )
{
   if( val > 0 ) return  1;
   if( val < 0 ) return -1;
   return 0;
}
//
// ===================================================================================
//
template < typename MetricalSpace >
typename gml::result_of::val< MetricalSpace >::type
orient2d_geomdim3 ( MetricalSpace const& pa,
                    MetricalSpace const& pb,
                    MetricalSpace const& pc )
{
   //std::cout << pa << " - " << pb << " - " << pc << std::endl;
   //std::cout << gsse::ex( pb-pa, pc-pa ) << std::endl;

  
   MetricalSpace ex_vector = gsse::ex( pb-pa, pc-pa );
   //
   // [JW] TODO verify: we dont need the expensive squared length ... 
   //
   if( gsse::length_square( ex_vector ) == 0 ) return 0; // it has to be on the line

   MetricalSpace ex_vector_norm = gsse::normalize( ex_vector );
   
   //std::cout << "normal: " << ex_vector_norm << std::endl;

   MetricalSpace temppoint(pa[0] + ex_vector_norm[0],
                           pa[1] + ex_vector_norm[1],
                           pa[2] + ex_vector_norm[2]);
                           
   return gml::algorithms::evaluate( 
      gml::algorithms::jrs_orient3dfast( pa, pb, temppoint, pc ) );
}
//
// ===================================================================================
//
template < int DIMG >
struct orient2d { };

template < >
struct orient2d < 2 >
{
   template < typename MetricalSpace >
   static typename gml::result_of::val< MetricalSpace >::type
   compute( MetricalSpace const& pa,
            MetricalSpace const& pb,
            MetricalSpace const& pc )
   {
      return gml::algorithms::jrs_orient2dfast( pa, pb, pc );
   }
   template < typename MetricalSpace >
   static typename gml::result_of::val< MetricalSpace >::type
   compute( MetricalSpace & pa,
            MetricalSpace & pb,
            MetricalSpace & pc )
   {
      return gml::algorithms::jrs_orient2dfast( pa, pb, pc );
   }   
};

template < >
struct orient2d < 3 >
{
   template < typename MetricalSpace >
   static typename gml::result_of::val< MetricalSpace >::type
   compute( MetricalSpace const& pa,
            MetricalSpace const& pb,
            MetricalSpace const& pc )
   {
      return gml::algorithms::orient2d_geomdim3( pa, pb, pc );
   }
   template < typename MetricalSpace >
   static typename gml::result_of::val< MetricalSpace >::type
   compute( MetricalSpace & pa,
            MetricalSpace & pb,
            MetricalSpace & pc )
   {
      return gml::algorithms::orient2d_geomdim3( pa, pb, pc );
   }   
};
//
// ===================================================================================
//
template < int DIMG >
struct point_in_triangle { };

template < >
struct point_in_triangle < 2 > 
{ 
   template < typename MetricalSpace >
   static int
   compute( MetricalSpace const& pa,
            MetricalSpace const& pb,
            MetricalSpace const& pc,
            MetricalSpace const& ptn )
   {
      // [JW] .. as [MS] implemented a different integer set than [RH]
      // I provide a mapping to transfer the [MS] set to [RH] set ..
      //
      int ms_result = gsse::orient2D_triangle( pa, pb, pc, ptn );
      if( ms_result == 0 ) return 1;
      if( ms_result == 2 ) return -1;
      return 0;
   }   
   template < typename MetricalSpace >
   static int
   compute( MetricalSpace & pa,
            MetricalSpace & pb,
            MetricalSpace & pc,
            MetricalSpace & ptn )
   {
      // [JW] .. as [MS] implemented a different integer set than [RH]
      // I provide a mapping to transfer the [MS] set to [RH] set ..
      //
      int ms_result = gsse::orient2D_triangle( pa, pb, pc, ptn );
      if( ms_result == 0 ) return 1;
      if( ms_result == 2 ) return -1;
      return 0;
   }      
};

template < >
struct point_in_triangle < 3 > 
{ 
   template < typename MetricalSpace >
   static int
   compute( MetricalSpace const& pa,
            MetricalSpace const& pb,
            MetricalSpace const& pc,
            MetricalSpace const& ptn )
   {
  
      return gsse::orient3d_triangle( pa, pb, pc, ptn );
   }   

   template < typename MetricalSpace >
   static int
   compute( MetricalSpace & pa,
            MetricalSpace & pb,
            MetricalSpace & pc,
            MetricalSpace & ptn )
   {
  
      return gsse::orient3d_triangle( pa, pb, pc, ptn );
   }   
};

//
// ===================================================================================
//
} // end namespace: algorithms

} // end namespace: gml

#endif






















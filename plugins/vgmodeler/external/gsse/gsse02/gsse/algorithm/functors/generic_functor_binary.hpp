/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GML_FUNCTORS_GENERICFUNCTORBINARY
#define GML_FUNCTORS_GENERICFUNCTORBINARY
//
// ===================================================================================
//
// *** GML includes
//
#include "../datastructure/metric_object_rt.hpp"
#include "../util/copy.hpp"
#include "../util/for_each.hpp"
#include "../util/is_tag.hpp"
#include "../util/size.hpp"
#include "../util/resize.hpp"
//
// *** GSSE includes
//
#include <gsse/topology/coboundary.hpp>
#include <gsse/geometry/metric_object.hpp>
#include <gsse/algorithm/io.hpp>
//
// *** BOOST includes
//
#include <boost/spirit/home/phoenix/core.hpp>  
//
// ===================================================================================
//
//#define DEBUG_GENERICFUNCTORBINARY
//
// ===================================================================================
//
namespace gml
{

//
// ===================================================================================
//
namespace detail
{
//
// ===================================================================================
//
template < typename TopologyOperation1,
           typename TopologyOperation2,
           typename Algorithm,
           typename AlgorithmResult,
           typename Property >
struct generic_functor_binary_impl 
{ 
   generic_functor_binary_impl ()
   {  
   #ifdef DEBUG_GENERICFUNCTORBINARY
      std::cout << "DEBUG::GENERIC-FUNCTOR-BINARY::DEFAULT::CONSTRUCTOR" << std::endl;
      std::cout << "WARNING::TOPOLOGY OPERATION NOT RECOGNIZED!" << std::endl;
   #endif
   }
};
//
// ===================================================================================
//
template < long DIM1, long DIMRel1, typename CellTopology1,
           long DIM2, long DIMRel2, typename CellTopology2,
           typename Algorithm,
           typename AlgorithmResult,
           typename Property >
struct generic_functor_binary_impl < gsse::boundary< DIM1, DIMRel1, CellTopology1 >,
                                     gsse::boundary< DIM2, DIMRel2, CellTopology2 >,
                                     Algorithm,
                                     AlgorithmResult,
                                     Property >
{  
   // ----------------------------------
   typedef gsse::boundary< DIM1, DIMRel1, CellTopology1 >                           TopologyOperation1;
   typedef typename gsse::result_of::boundary< DIM1, DIMRel1, CellTopology1 >::type TopologyOperationResult1;      
   typedef gsse::boundary< DIM2, DIMRel2, CellTopology2 >                           TopologyOperation2;
   typedef typename gsse::result_of::boundary< DIM2, DIMRel2, CellTopology2 >::type TopologyOperationResult2;      
   typedef AlgorithmResult                                                          result_type;

   static const long DIMResult1 = DIM1 - DIMRel1;
   static const long DIMResult2 = DIM2 - DIMRel2;
   // ----------------------------------
   // ==================================
   //
   // CONSTRUCTOR - PART
   //
   generic_functor_binary_impl()
   {  
   #ifdef DEBUG_GENERICFUNCTORBINARY
      std::cout << "DEBUG::GENERIC-FUNCTOR-BINARY::BOUNDARY::CONSTRUCTOR" << std::endl;
   #endif
   }
   // ================================== 
   // 
   // OPERATOR PART
   //
   template < 
              typename TopologyElement1,
              typename Geometry1, 
              typename TopologyElement2,              
              typename Geometry2 >
   AlgorithmResult
   operator()( TopologyElement1  const& ele1,
               Geometry1         const& geometry1, 
               TopologyElement2  const& ele2,
               Geometry2         const& geometry2 )
   {
      AlgorithmResult   algorithm_result;
      // ----------------------------------     
      namespace phoenix = boost::phoenix; 
      using namespace phoenix::arg_names;
      using namespace phoenix::local_names;      
      // ----------------------------------     
      typename gml::result_of::create_conform_cell< TopologyElement1 >::type     conform_topo_ele1;
      typename gml::result_of::create_conform_cell< TopologyElement2 >::type     conform_topo_ele2;
      // ----------------------------------     
      gml::resize( gml::size(ele1) )( conform_topo_ele1 );
      gml::resize( gml::size(ele2) )( conform_topo_ele2 );
      // ----------------------------------     
      typedef typename gml::result_of::val< Geometry1 >::type                             GeometryElement1;
      typedef typename gml::result_of::create_conform_metric< GeometryElement1 >::type    ConformMetricObject1;

      typedef typename gml::result_of::val< Geometry2 >::type                             GeometryElement2;
      typedef typename gml::result_of::create_conform_metric< GeometryElement2 >::type    ConformMetricObject2;

      typedef typename gsse::array< ConformMetricObject1, DIMResult1+1 >      ConformGeometryCell1;
      ConformGeometryCell1 conform_geometry_cell1;  
      typedef typename gsse::array< ConformMetricObject2, DIMResult2+1 >      ConformGeometryCell2;
      ConformGeometryCell2 conform_geometry_cell2;  
      // ----------------------------------     
      gsse::traverse()
      [
         gml::functional::resize( _1, gml::functional::size( phoenix::at( phoenix::ref(geometry1), 0 ) ) )            
      ](conform_geometry_cell1);
      gsse::traverse()
      [
         gml::functional::resize( _1, gml::functional::size( phoenix::at( phoenix::ref(geometry2), 0 ) ) )            
      ](conform_geometry_cell2);      
      // ----------------------------------           
      gml::copy( ele1, conform_topo_ele1 );
      gml::copy( ele2, conform_topo_ele2 );
      // ----------------------------------  
      TopologyOperationResult1 topo_op_result1 = _topo_operation1( conform_topo_ele1 );      
      TopologyOperationResult2 topo_op_result2 = _topo_operation2( conform_topo_ele2 );      
      // ----------------------------------  
      std::vector< ConformGeometryCell2 >     algorithm_cell2_cont;
      int ci;
      gsse::traverse()
      [
         phoenix::ref(ci) = 0,
         gsse::traverse()
         [
            gml::functional::copy( 
               phoenix::at( phoenix::ref(geometry2),       _1),
               phoenix::at( phoenix::ref(conform_geometry_cell2), phoenix::ref(ci) ) ),
            phoenix::ref(ci)++
         ],
         phoenix::push_back( phoenix::ref(algorithm_cell2_cont), phoenix::ref(conform_geometry_cell2) )
      ](topo_op_result2);      
      // ----------------------------------        
      gsse::traverse()
      [
         phoenix::ref(ci) = 0,
         gsse::traverse()
         [
            gml::functional::copy( 
               phoenix::at( phoenix::ref(geometry1),       _1),
               phoenix::at( phoenix::ref(conform_geometry_cell1), phoenix::ref(ci) ) ),
            phoenix::ref(ci)++
         ],      
         phoenix::let( _a = _1 )
         [
            phoenix::for_each( 
               phoenix::ref(algorithm_cell2_cont), 
               phoenix::lambda
               [
               #ifdef DEBUG_GENERICFUNCTORBINARY
                  std::cout << phoenix::val("bnd-res1-ele: ") << phoenix::ref(conform_geometry_cell1) << phoenix::val(" || ") 
                     << phoenix::val("bnd-res2-ele: ") << _1 << std::endl,
               #endif
                  phoenix::if_( _algorithm( phoenix::ref(conform_geometry_cell1), _1 ) == 1 )
                  [
                     phoenix::push_back( phoenix::ref(algorithm_result), _a )   
                  ]
               ]         
            )
         ]
      ](topo_op_result1);          
   #ifdef DEBUG_GENERICFUNCTORBINARY      
      gml::for_each ( algorithm_result, std::cout << phoenix::val("  ") << _1 << std::endl );
   #endif      
      // ----------------------------------    
      return algorithm_result;
      // ----------------------------------             
   }
   // ----------------------------------
   TopologyOperation1      _topo_operation1;   
   TopologyOperation2      _topo_operation2;   
   Algorithm               _algorithm;
   // ----------------------------------
};

//
// ===================================================================================
//
} // end namespace: detail

template < typename Algorithm,
           typename AlgorithmResult,
           typename Property >   
struct generic_functor_binary
{
   typedef gml::detail::generic_functor_binary_impl<
      typename Property::topology_operation_1_type,
      typename Property::topology_operation_2_type,      
      Algorithm,
      AlgorithmResult,
      Property
   >  type;  
};

//
// ===================================================================================
//
} // end namespace: gml


#endif

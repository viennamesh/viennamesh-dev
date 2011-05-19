/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GML_FUNCTORS_GENERICFUNCTORUNARY
#define GML_FUNCTORS_GENERICFUNCTORUNARY
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
//
// *** BOOST includes
//
#include <boost/spirit/home/phoenix/core.hpp>  
//
// ===================================================================================
//
//#define DEBUG_GENERICFUNCTORUNARY
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
template < typename TopologyOperation,
           typename Algorithm,
           typename AlgorithmResult,
           typename Property >
struct generic_functor_unary_impl 
{ 
   generic_functor_unary_impl()
   {  
   #ifdef DEBUG_GENERICFUNCTORUNARY
      std::cout << "DEBUG::GENERIC-FUNCTOR-UNARY::DEFAULT::CONSTRUCTOR" << std::endl;
      std::cout << "WARNING::TOPOLOGY OPERATION NOT RECOGNIZED!" << std::endl;
   #endif
   }
};
//
// ===================================================================================
//
template < long DIM, long DIMRel, typename CellTopology,
           typename Algorithm,
           typename AlgorithmResult,
           typename Property >
struct generic_functor_unary_impl < gsse::boundary< DIM, DIMRel, CellTopology >,
                                    Algorithm,
                                    AlgorithmResult,
                                    Property >
{  
   // ----------------------------------
   typedef gsse::boundary< DIM, DIMRel, CellTopology >                              TopologyOperation;
   typedef typename gsse::result_of::boundary< DIM, DIMRel, CellTopology >::type    TopologyOperationResult;      
   typedef AlgorithmResult                                                          result_type;

   static const long DIMResult = DIM - DIMRel;
   // ----------------------------------
   // ==================================
   //
   // CONSTRUCTOR - PART
   //
   generic_functor_unary_impl()
   {  
   #ifdef DEBUG_GENERICFUNCTORUNARY
      std::cout << "DEBUG::UNARY-GENERIC-FUNCTOR::BOUNDARY::CONSTRUCTOR" << std::endl;
   #endif
   }
   // ================================== 
   // 
   // OPERATOR PART
   //
   template < typename Geometry, typename TopologyElement >
   AlgorithmResult
   operator()( Geometry const& geometry, TopologyElement const& ele )
   {
      AlgorithmResult   algorithm_result;
      // ----------------------------------     
      namespace phoenix = boost::phoenix; 
      using namespace phoenix::arg_names;
      // ----------------------------------     
      typename gml::result_of::create_conform_cell< TopologyElement >::type     conform_topo_ele;

      gml::resize( gml::size(ele) )( conform_topo_ele );
      // ----------------------------------     
      typedef typename gml::result_of::val< Geometry >::type                           GeometryElement;
      typedef typename gml::result_of::create_conform_metric< GeometryElement >::type  ConformMetricObject;

      typename gsse::array< ConformMetricObject, DIMResult+1 >   conform_geometry_cell;  
      // ----------------------------------     
      gsse::traverse()
      [
         gml::functional::resize( _1, gml::functional::size( phoenix::at( phoenix::ref(geometry), 0 ) ) )            
      ](conform_geometry_cell);
      // ----------------------------------           
      gml::copy( ele, conform_topo_ele );
      // ----------------------------------  
      TopologyOperationResult topo_op_result = _topo_operation( conform_topo_ele );      
      // ----------------------------------  
      int ci;
      gsse::traverse()
      [
         //
         // we build the algorithm_cell: 
         // array where each element is a coordinate of the cells vertex.
         //
         phoenix::ref(ci) = 0,
         gsse::traverse()
         [
            // using a phoenix::lazy_function wrapper for the gml::copy implementation
            //
            gml::functional::copy( 
               phoenix::at( phoenix::ref(geometry),               _1),
               phoenix::at( phoenix::ref(conform_geometry_cell),  phoenix::ref(ci) ) ),
            phoenix::ref(ci)++
         ],
         //
         // pass the previously created algorithm_cell to the algorithm and store the result
         // on the algorithm_result container
         //
         phoenix::push_back( 
            phoenix::ref(algorithm_result), 
            _algorithm( phoenix::ref(conform_geometry_cell) ) 
         )
      ](topo_op_result);      
      // ----------------------------------    
   #ifdef DEBUG_GENERICFUNCTORUNARY      
      gml::for_each ( algorithm_result, std::cout << phoenix::val("  ") << _1 << std::endl );
   #endif
      // ----------------------------------    
      return algorithm_result;
      // ----------------------------------             
   }
   // ----------------------------------
   TopologyOperation       _topo_operation;   
   Algorithm               _algorithm;
   // ----------------------------------
};
//
// ===================================================================================
//
template < long DIM, long DIMRel, typename CellTopology,
           typename Algorithm,
           typename AlgorithmResult,
           typename Property >
struct generic_functor_unary_impl < gsse::coboundary< DIM, DIMRel, CellTopology >,
                                    Algorithm,
                                    AlgorithmResult,
                                    Property >
{  
   // ----------------------------------
   typedef gsse::boundary< DIM, DIMRel, CellTopology >                              TopologyOperation;
   typedef typename gsse::result_of::coboundary< DIM, DIMRel, CellTopology >::type  TopologyOperationResult;   
   typedef AlgorithmResult                                                          result_type;

   static const long DIMResult = DIM + DIMRel;
   // ----------------------------------
   // ==================================
   //
   // CONSTRUCTOR - PART
   //
   generic_functor_unary_impl(typename Property::topology_container_type& cell_container)
      : _cell_container( cell_container)
   {  
   #ifdef DEBUG_GENERICFUNCTORUNARY
      std::cout << "DEBUG::UNARY-GENERIC-FUNCTOR::COBOUNDARY::CONSTRUCTOR" << std::endl;
   #endif
   }
   // ================================== 
   // 
   // OPERATOR PART
   //
   template < typename Geometry, typename TopologyElement >
   AlgorithmResult
   operator()( Geometry const& geometry, TopologyElement const& ele )
   {
      AlgorithmResult   algorithm_result;
      // ----------------------------------     
      namespace phoenix = boost::phoenix; 
      using namespace phoenix::arg_names;
      // ----------------------------------     
      typename gml::result_of::create_conform_cell< TopologyElement >::type     conform_topo_ele;

      gml::resize( gml::size(ele) )( conform_topo_ele );
      // ----------------------------------     
      typedef typename gml::result_of::val< Geometry >::type                           GeometryElement;
      typedef typename gml::result_of::create_conform_metric< GeometryElement >::type  ConformMetricObject;

      typename gsse::array< ConformMetricObject, DIMResult+1 >   conform_geometry_cell;  
      // ----------------------------------     
      gsse::traverse()
      [
         gml::functional::resize( _1, gml::functional::size( phoenix::at( phoenix::ref(geometry), 0 ) ) )            
      ](conform_geometry_cell);
      // ----------------------------------           
      gml::copy( ele, conform_topo_ele );
      // ----------------------------------  
      TopologyOperationResult topo_op_result = _topo_operation( conform_topo_ele );      
      // ----------------------------------  
      int ci;
      gsse::traverse()
      [
         //
         // we build the algorithm_cell: 
         // array where each element is a coordinate of the cells vertex.
         //
         phoenix::ref(ci) = 0,
         gsse::traverse()
         [
            // using a phoenix::lazy_function wrapper for the gml::copy implementation
            //
            gml::functional::copy( 
               phoenix::at( phoenix::ref(geometry),               _1),
               phoenix::at( phoenix::ref(conform_geometry_cell),  phoenix::ref(ci) ) ),
            phoenix::ref(ci)++
         ],
         //
         // pass the previously created algorithm_cell to the algorithm and store the result
         // on the algorithm_result container
         //
         phoenix::push_back( 
            phoenix::ref(algorithm_result), 
            _algorithm( phoenix::ref(conform_geometry_cell) ) 
         )
      ](topo_op_result);      
      // ----------------------------------    
   #ifdef DEBUG_GENERICFUNCTORUNARY      
      gml::for_each ( algorithm_result, std::cout << phoenix::val("  ") << _1 << std::endl );
   #endif
      // ----------------------------------    
      return algorithm_result;
      // ----------------------------------             
   }
   // ----------------------------------
   TopologyOperation       _topo_operation;   
   Algorithm               _algorithm;
   
   typename Property::topology_container_type & _cell_container;
   // ----------------------------------
};
//
// ===================================================================================
//
} // end namespace: detail

template < typename Algorithm,
           typename AlgorithmResult,
           typename Property >   
struct generic_functor_unary
{
   typedef gml::detail::generic_functor_unary_impl<
      typename Property::topology_operation_type,
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

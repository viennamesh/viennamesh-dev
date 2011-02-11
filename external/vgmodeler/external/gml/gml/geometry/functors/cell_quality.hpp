/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GML_GEOMETRY_FUNCTORS_CELLQUALITY
#define GML_GEOMETRY_FUNCTORS_CELLQUALITY
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
#include "metric_quantity.hpp"
#include "../../algorithms/root_mean_square.hpp"
//
// *** GSSE includes
//
#include <gsse/topology/coboundary.hpp>
//
// ===================================================================================
//
//#define DEBUG_CELLQUALITY
//
// ===================================================================================
//
namespace gml {

//
// ===================================================================================
//
namespace result_of {

template < typename Numeric >
struct cell_quality
{
   typedef std::vector< Numeric >   type;
};

} // end namespace: result_of
//
// ===================================================================================
//
namespace detail {
template < int DIMTResult, typename CellTopology >
struct cell_quality_impl { };
// ===================================================================================
template < typename CellTopology >
struct cell_quality_impl < 2, CellTopology >
{
   // ------------------------------------------
   // [JW] we need to provide a template for each parameter ..
   // note: its a boost::phoenix::function - prerequesite!
   //
   template < typename Container1, typename Container2 >
   struct result
   { 
      typedef typename gml::result_of::val< Container1 >::type type;
   };
   // ------------------------------------------
   template < typename Container1, typename Container2 >
   typename result< Container1, Container2 >::type
   operator()(Container1 const& area_cont, Container2 const& length_cont ) const
   {
      typedef typename result< Container1, Container2 >::type  return_type;
      //   ( area / rms_length^2 ) * ( 4/sqrt(3) )
      return_type rms_length = 
         gml::root_mean_square< return_type >(length_cont);
      return fabs(( area_cont[0] / (rms_length * rms_length) ) * 2.309401077);
   }
   // ------------------------------------------   
};
// ===================================================================================
template < typename CellTopology >
struct cell_quality_impl < 3, CellTopology >
{
   // ------------------------------------------
   template < typename Container1, typename Container2 >
   struct result
   { 
      typedef typename gml::result_of::val< Container1 >::type type;
   };
   // ------------------------------------------
   template < typename Container1, typename Container2 >
   typename result< Container1, Container2 >::type
   operator()(Container1 const& volume_cont, Container2 const& length_cont ) const
   {
      typedef typename result< Container1, Container2 >::type  return_type;   
      //   ( volume / rms_length^3 ) * ( 6*sqrt(2) )
      return_type rms_length = 
         gml::root_mean_square< return_type >(length_cont);
      return fabs(( volume_cont[0] / (rms_length * rms_length * rms_length) ) * 8.485281374);
   }
   // ------------------------------------------   
};
//
// ===================================================================================
//
template < typename TopologyOperation,
           typename Algorithm,
           typename AlgorithmResult,
           typename Property >
struct cell_quality_functor_impl 
{ 
   cell_quality_functor_impl()
   {  
   #ifdef DEBUG_CELLQUALITY
      std::cout << "DEBUG::CELL-QUALITY-FUNCTOR::DEFAULT::CONSTRUCTOR" << std::endl;
      std::cout << "WARNING::TOPOLOGY OPERATION NOT RECOGNIZED!" << std::endl;
   #endif
   }
};
template < long DIM, long DIMRel, typename CellTopology,
           typename Algorithm,
           typename AlgorithmResult,
           typename Property >
struct cell_quality_functor_impl  < gsse::boundary< DIM, DIMRel, CellTopology >,
                                    Algorithm,
                                    AlgorithmResult,
                                    Property >
{ 
   // ----------------------------------
   typedef gsse::boundary< DIM, DIMRel, CellTopology >                              TopologyOperation;
   typedef typename gsse::result_of::boundary< DIM, DIMRel, CellTopology >::type    TopologyOperationResult;      
   typedef AlgorithmResult                                                          result_type;
   typedef typename Property::topology_container_type                               CellComplex;
   
   static const long DIMResult = DIM - DIMRel;
   
   typedef gsse::boundary< DIMResult, 0, CellTopology >                             TopoOpSize;
   typedef gml::property_data< TopoOpSize, CellComplex >                            PropertySize;
   typedef typename gml::metric_quantity< PropertySize >::type                      Size;

   typedef gsse::boundary< DIMResult, DIMResult-1, CellTopology >                   TopoOpLength;
   typedef gml::property_data< TopoOpLength, CellComplex >                          PropertyLength;
   typedef typename gml::metric_quantity< PropertyLength >::type                    Lengths;
   // ----------------------------------
   // ==================================
   //
   // CONSTRUCTOR - PART
   //
   cell_quality_functor_impl()
   {  
   #ifdef DEBUG_CELLQUALITY
      std::cout << "DEBUG::CELL-QUALITY-FUNCTOR::BOUNDARY::CONSTRUCTOR" << std::endl;
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

      for( typename TopologyOperationResult::iterator iter = topo_op_result.begin();
           iter != topo_op_result.end(); iter++ )
      {  
         algorithm_result.push_back( 
            _algorithm( 
               _cell_size(geometry, *iter), 
               _cell_edge_lengths(geometry, *iter) 
            ) 
         );
      } 

      /*
      gsse::traverse()
      [
         _cell_size( phoenix::cref(geometry), _1)
         //phoenix::push_back( 
         //   phoenix::ref(algorithm_result), 
         //   _algorithm( 
         //      _cell_size( phoenix::cref(geometry), _1), 
         //      _cell_edge_lengths( phoenix::cref(geometry), _1) 
         //   ) 
         //)
      ](topo_op_result);      
      */
      // ----------------------------------    
   #ifdef DEBUG_CELLQUALITY      
      gml::for_each ( algorithm_result, std::cout << phoenix::val("  ") << _1 << std::endl );
   #endif
      // ----------------------------------    
      return algorithm_result;
      // ----------------------------------             
   }
   // ----------------------------------
   TopologyOperation       _topo_operation;   
   Algorithm               _algorithm;
   Size                    _cell_size;
   Lengths                 _cell_edge_lengths;
   // ----------------------------------
};
} // end namespace: detail
//
// ===================================================================================
//

template < typename Algorithm,
           typename AlgorithmResult,
           typename Property >   
struct cell_quality_functor
{
   typedef gml::detail::cell_quality_functor_impl<
      typename Property::topology_operation_type,
      Algorithm,
      AlgorithmResult,
      Property
   >  type;  
};
//
// ===================================================================================
//
template < typename NonEvaluatedProperty >
struct cell_quality
{
   typedef typename NonEvaluatedProperty::type    Property;

   typedef typename gml::cell_quality_functor < 
      //boost::phoenix::function< 
         gml::detail::cell_quality_impl<
            Property::DIMResult,
            typename Property::cell_topology
         >, 
      //>,
      typename gml::result_of::cell_quality< double >::type,
      Property
   >::type  type;      
};
//
// ===================================================================================
//


} // end namespace: gml

#endif






















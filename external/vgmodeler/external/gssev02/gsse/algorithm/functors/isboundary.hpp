/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GML_FUNCTORS_ISBOUNDARY
#define GML_FUNCTORS_ISBOUNDARY
//
// ===================================================================================
//
// *** GSSE include
//
#include <gsse/topology/coboundary.hpp>
//
// *** BOOST include
//
#include <boost/fusion/include/is_sequence.hpp>
//
// ===================================================================================
//
namespace gml 
{

namespace detail {

template < int      DIMElement,
           int      DIMCellComplex,
           typename CellTopology >
struct is_boundary_impl {};

template < int      DIMCellComplex,            
           typename CellTopology >
struct is_boundary_impl < 0, DIMCellComplex, CellTopology > // Vertex
{
   template < typename TopoElement,
              typename VertexFiberBundle,
              typename CellContainer >   
   static bool
   execute    (TopoElement     const& ele,
               VertexFiberBundle    & vertex_cont,
               CellContainer        & cell_cont) 
   {
      typedef  gsse::property_data<DIMCellComplex, CellTopology>              Property;   
   
      typedef typename gsse::result_of::edge_on_vertex<Property>::type        EdgeOnVertex;
      typedef typename gsse::result_of::edge_on_vertex<Property>::result      EdgeOnVertex_result;

      typedef typename gsse::result_of::cell_on_edge<Property>::type          CellOnEdge;
      typedef typename gsse::result_of::cell_on_edge<Property>::result        CellOnEdge_result;

      EdgeOnVertex         edge_on_vertex;
      EdgeOnVertex_result  edge_result = edge_on_vertex( ele, vertex_cont, cell_cont );


      //
      // -------------------------------------------------------------------------
      //
      //  IMPOSING UNIQUENESS ON THE EDGE RESULT CONTAINER ..
      //
      typedef std::map< typename gml::result_of::val< EdgeOnVertex_result >::type, bool > UniqueEdgeSet;
      UniqueEdgeSet   edge_result_unique;

      for( long ei = 0; ei < gsse::size( edge_result ); ei++ )
      {
         std::sort( edge_result[ei].begin(), edge_result[ei].begin()+2 );

         if( !edge_result_unique[edge_result[ei]] )
            edge_result_unique[edge_result[ei]] = true;
      }      
      
      long edge_size_unique = gsse::size( edge_result_unique );
      
      edge_result.clear();
      gsse::resize( edge_size_unique )( edge_result );
      
      long ei = 0;
      for( typename UniqueEdgeSet::iterator iter = edge_result_unique.begin(); 
           iter != edge_result_unique.end(); iter++ )
      {      
         gml::copy( iter->first, edge_result[ei] );
         ei++;
      }
      //
      // -------------------------------------------------------------------------
      //   

      CellOnEdge           cell_on_edge;
      for( long ei = 0; ei < gsse::size( edge_result ); ei++ )
      {
         CellOnEdge_result cell_result = cell_on_edge( edge_result[ei], vertex_cont, cell_cont );
         if( gsse::size( cell_result ) == 1 ) return true;
      }

      return false;
   }
   // ===========================================================
   template < typename TopoElement,
              typename VertexFiberBundle,
              typename CellContainer,
              typename GlobalLocalIndexMap >   
   static bool
   execute    (TopoElement     const& ele,
               VertexFiberBundle    & vertex_cont,
               CellContainer        & cell_cont,
               GlobalLocalIndexMap  & global_local) 
   {
      typedef  gsse::property_data<DIMCellComplex, CellTopology>              Property;   
   
      typedef typename gsse::result_of::edge_on_vertex<Property>::type        EdgeOnVertex;
      typedef typename gsse::result_of::edge_on_vertex<Property>::result      EdgeOnVertex_result;

      typedef typename gsse::result_of::cell_on_edge<Property>::type          CellOnEdge;
      typedef typename gsse::result_of::cell_on_edge<Property>::result        CellOnEdge_result;

      EdgeOnVertex         edge_on_vertex;
      
      EdgeOnVertex_result  edge_result = 
         edge_on_vertex( ele, vertex_cont, cell_cont, global_local );
      //
      // -------------------------------------------------------------------------
      //
      //  IMPOSING UNIQUENESS ON THE EDGE RESULT CONTAINER ..
      //
      typedef std::map< typename gml::result_of::val< EdgeOnVertex_result >::type, bool > UniqueEdgeSet;
      UniqueEdgeSet   edge_result_unique;

      for( long ei = 0; ei < gsse::size( edge_result ); ei++ )
      {
         std::sort( edge_result[ei].begin(), edge_result[ei].begin()+2 );

         if( !edge_result_unique[edge_result[ei]] )
            edge_result_unique[edge_result[ei]] = true;
      }      
      
      long edge_size_unique = gsse::size( edge_result_unique );
      
      edge_result.clear();
      gsse::resize( edge_size_unique )( edge_result );
      
      long ei = 0;
      for( typename UniqueEdgeSet::iterator iter = edge_result_unique.begin(); 
           iter != edge_result_unique.end(); iter++ )
      {      
         gml::copy( iter->first, edge_result[ei] );
         ei++;
      }
      //
      // -------------------------------------------------------------------------
      //   

      CellOnEdge           cell_on_edge;
      for( long ei = 0; ei < gsse::size( edge_result ); ei++ )
      {
         CellOnEdge_result cell_result = 
            cell_on_edge( edge_result[ei], vertex_cont, cell_cont, global_local );
         if( gsse::size( cell_result ) == 1 ) return true;
      }

      return false;
   }
};

template < int      DIMCellComplex,            
           typename CellTopology >
struct is_boundary_impl < 1, DIMCellComplex, CellTopology > // Edge
{
   template < typename TopoElement,
              typename VertexFiberBundle,
              typename CellContainer>   
   static bool
   execute    (TopoElement     const& ele,
               VertexFiberBundle    & vertex_cont,
               CellContainer        & cell_cont) 
   {
      typedef  gsse::property_data<DIMCellComplex, CellTopology>              Property;
   
      typedef typename gsse::result_of::cell_on_edge<Property>::type          CellOnEdge;
      typedef typename gsse::result_of::cell_on_edge<Property>::result        CellOnEdge_result;

      CellOnEdge           cell_on_edge;

      CellOnEdge_result cell_result = cell_on_edge( ele, vertex_cont, cell_cont );
      if( gsse::size( cell_result ) == 1 ) return true;

      return false;
   }
   // ===========================================================
   template < typename TopoElement,
              typename VertexFiberBundle,
              typename CellContainer,
              typename GlobalLocalIndexMap >   
   static bool
   execute    (TopoElement     const& ele,
               VertexFiberBundle    & vertex_cont,
               CellContainer        & cell_cont,
               GlobalLocalIndexMap  & global_local) 
   {
      typedef  gsse::property_data<DIMCellComplex, CellTopology>              Property;
   
      typedef typename gsse::result_of::cell_on_edge<Property>::type          CellOnEdge;
      typedef typename gsse::result_of::cell_on_edge<Property>::result        CellOnEdge_result;

      CellOnEdge           cell_on_edge;

      CellOnEdge_result cell_result = cell_on_edge( ele, vertex_cont, cell_cont, global_local );
      if( gsse::size( cell_result ) == 1 ) return true;

      return false;
   }
};

template < int      DIMCellComplex,            
           typename CellTopology >
struct is_boundary_impl < 2, DIMCellComplex, CellTopology > // Facet
{
   template < typename TopoElement,
              typename VertexFiberBundle,
              typename CellContainer>   
   static bool
   execute    (TopoElement     const& ele,
               VertexFiberBundle    & vertex_cont,
               CellContainer        & cell_cont) 
   {
      typedef  gsse::property_data<DIMCellComplex, CellTopology>              Property;
   
      typedef typename gsse::result_of::cell_on_facet<Property>::type          CellOnFacet;
      typedef typename gsse::result_of::cell_on_facet<Property>::result        CellOnFacet_result;

      CellOnFacet           cell_on_facet;
      
      CellOnFacet_result cell_result = cell_on_facet( ele, vertex_cont, cell_cont );

      if( gsse::size( cell_result ) == 1 ) return true;

      return false;
   }
   // ===========================================================
   template < typename TopoElement,
              typename VertexFiberBundle,
              typename CellContainer,
              typename GlobalLocalIndexMap >   
   static bool
   execute    (TopoElement     const& ele,
               VertexFiberBundle    & vertex_cont,
               CellContainer        & cell_cont,
               GlobalLocalIndexMap  & global_local) 
   {
      typedef  gsse::property_data<DIMCellComplex, CellTopology>              Property;
   
      typedef typename gsse::result_of::cell_on_facet<Property>::type          CellOnFacet;
      typedef typename gsse::result_of::cell_on_facet<Property>::result        CellOnFacet_result;

      CellOnFacet           cell_on_facet;
      
      CellOnFacet_result cell_result = cell_on_facet( ele, vertex_cont, cell_cont, global_local );

      if( gsse::size( cell_result ) == 1 ) return true;

      return false;
   }
};


} // end namespace: detail

template < int DIM, 
           typename CellTopology >
struct is_boundary
{
   // ------------------------------------------------   
   typedef bool result_type;
   // ------------------------------------------------   
   template < typename TopoElement,
              typename VertexFiberBundle,
              typename CellContainer >   
   inline result_type
   operator() (TopoElement     const& ele,
               VertexFiberBundle    & vertex_cont,
               CellContainer        & cell_cont) 
   {  
      return gml::detail::is_boundary_impl< 
         gml::result_of::size< TopoElement >::value-1,
         DIM,
         CellTopology
      >::execute(ele, vertex_cont, cell_cont);   
   }      
   // ------------------------------------------------   
   template < typename TopoElement,
              typename VertexFiberBundle,
              typename CellContainer,
              typename GlobalLocalIndexMap >   
   inline result_type
   operator() (TopoElement     const& ele,
               VertexFiberBundle    & vertex_cont,
               CellContainer        & cell_cont,
               GlobalLocalIndexMap  & global_local) 
   {  
      return gml::detail::is_boundary_impl< 
         gml::result_of::size< TopoElement >::value-1,
         DIM,
         CellTopology
      >::execute(ele, vertex_cont, cell_cont, global_local);   
   }      
   // ------------------------------------------------     
};



//
// ==================================================================================
//
} // end namespace: gml
#endif
















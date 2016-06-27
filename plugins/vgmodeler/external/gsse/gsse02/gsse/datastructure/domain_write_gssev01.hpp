/* ============================================================================
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
#ifndef GSSE_INTERFACE_GSSE_WRITEDOMAIN
#define GSSE_INTERFACE_GSSE_WRITEDOMAIN
//
// ===================================================================================
//
// *** GSSE includes
//
#include <gsse/datastructure/domain.hpp>
//
// *** SYSTEM includes
//
#include <iomanip>
//
// ===================================================================================
//

namespace gsse {

template < typename Domain >
void write_domain_gssev01(Domain & domain, std::string const& filename)
{
   using namespace gsse::access_specifier; 
   typedef gsse::property_domain<Domain>           DomainPropertyT;
   
   typedef typename gsse::result_of::property_SpaceTopologySegmentsT<DomainPropertyT>::type  SpaceTopologySegmentsT;
   typedef typename gsse::result_of::property_SpaceQuantitySegmentsT<DomainPropertyT>::type  SpaceQuantitySegmentsT;  
   typedef typename gsse::result_of::property_FBPosT<DomainPropertyT>::type                  FBPosT;    
   typedef typename gsse::result_of::property_CoordPosT<DomainPropertyT>::type               CoordPosT;     
   typedef typename gsse::result_of::at_dim_index<SpaceQuantitySegmentsT>::type              QuanComplexT;      

   const long DIMG = gsse::result_of::property_DIMG<DomainPropertyT>::value;   
   
   SpaceTopologySegmentsT&  segments_topology = gsse::at_dim< AC >(domain); 
   SpaceQuantitySegmentsT&  segments_quantity = gsse::at_dim< AQ >(domain);  
   FBPosT&                  geometry          = gsse::at_dim< AP >(domain);        
   // -----------------------------------
   std::ofstream  stream; 
   stream.open(filename.c_str());
   // -----------------------------------
   stream << DIMG << std::endl;
   stream << gsse::size( geometry ) << std::endl;
   stream.setf( std::ios::right,std::ios::adjustfield);
   stream << std::setprecision(12)  << std::setiosflags(std::ios::fixed);         
   // -----------------------------------   
   for( long i = 0; i < gsse::size( geometry ); i++ )
   {
      // -----------------------------------
      stream << gsse::at(i)(geometry) << std::endl;
      // -----------------------------------
   }
   // -----------------------------------
   stream << std::endl;
   // -----------------------------------
   stream << gsse::size(segments_topology) << std::endl;
   for( long si = 0; si < gsse::size(segments_topology); si++ )
   {
      // -----------------------------------
      stream << "Default-Segment-" + boost::lexical_cast<std::string>(si) << std::endl;
      stream << gsse::size( gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_topology))) ) << std::endl;
      stream << gsse::size( gsse::at_dim<AT_vx>(gsse::at_fiber(gsse::at(si)(segments_topology))) ) << std::endl;
      // -----------------------------------
      // Topology Part
      //      
      for( long ci = 0; ci < gsse::size( gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_topology))) ); ci++ )
      {
         // -----------------------------------
         stream << ci << " " << 
            gsse::at(ci)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_topology))) )
         << std::endl;
         // -----------------------------------
      }
      // -----------------------------------
      // Vertex Quantity Part
      //
      long vertex_quantity_size = gsse::size( 
         gsse::at_dim<AT_vx>(gsse::at_fiber(gsse::at(si)(segments_quantity))) );
      if( vertex_quantity_size > 0 )
      {
         typedef typename gsse::result_of::at_dim<QuanComplexT, AT_vx>::type  VertexQuantityComplexT;
         typedef typename VertexQuantityComplexT::iterator                    VertexQuantityIteratorT;
         
         typedef typename VertexQuantityComplexT::value_type::second_type     VertexQuantityT;
         typedef typename VertexQuantityT::iterator                           DataIteratorT;
         
         stream << "Quantity: sheaf_vertex " << vertex_quantity_size << std::endl;
         
         VertexQuantityIteratorT begin = 
            gsse::at_dim<AT_vx>(gsse::at_fiber(gsse::at(si)(segments_quantity))).begin();

         VertexQuantityIteratorT end = 
            gsse::at_dim<AT_vx>(gsse::at_fiber(gsse::at(si)(segments_quantity))).end();
         
         for( VertexQuantityIteratorT quan_iter = begin; quan_iter != end; ++quan_iter  )
         {
            long x = 1;
            long y = gsse::size((*(((*quan_iter).second).begin())).second);
            stream << (*quan_iter).first 
                   << " type " << x << " " << y << std::endl; 
            stream << gsse::size((*quan_iter).second) << std::endl;
            
            for( DataIteratorT data_iter = ((*quan_iter).second).begin();
                 data_iter != ((*quan_iter).second).end(); ++data_iter ) 
            {
               stream << (*data_iter).first << " ";
               for( long i = 0; i < gsse::size((*data_iter).second); i++ )
               {
                  stream << ((*data_iter).second)[i] << " ";
               }
               stream << std::endl;
            }
         }
      }
      // -----------------------------------
      // CELL Quantity Part
      //
      long cell_quantity_size = gsse::size( 
         gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))) );
      if( cell_quantity_size > 0 )
      {
         typedef typename gsse::result_of::at_dim<QuanComplexT, AT_cl>::type  CellQuantityComplexT;
         typedef typename CellQuantityComplexT::iterator                      CellQuantityIteratorT;
         
         typedef typename CellQuantityIteratorT::value_type::second_type      CellQuantityT;
         typedef typename CellQuantityT::iterator                             DataIteratorT;
         
         stream << "Quantity: sheaf_cell " << cell_quantity_size << std::endl;
         
         CellQuantityIteratorT begin = 
            gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))).begin();

         CellQuantityIteratorT end = 
            gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_quantity))).end();
         
         for( CellQuantityIteratorT quan_iter = begin; quan_iter != end; ++quan_iter  )
         {
            long x = 1;
            long y = gsse::size((*(((*quan_iter).second).begin())).second);
            stream << (*quan_iter).first 
                   << " type " << x << " " << y << std::endl; 
            stream << gsse::size((*quan_iter).second) << std::endl;
            
            for( DataIteratorT data_iter = ((*quan_iter).second).begin();
                 data_iter != ((*quan_iter).second).end(); ++data_iter ) 
            {
               stream << (*data_iter).first << " ";
               for( long i = 0; i < gsse::size((*data_iter).second); i++ )
               {
                  stream << ((*data_iter).second)[i] << " ";
               }
               stream << std::endl;
            }
         }
      }      
      // -----------------------------------
      stream << std::endl;
      // -----------------------------------
   }
   // -----------------------------------
   stream.close();   
   // -----------------------------------
}


} // end namespace: gsse
#endif


















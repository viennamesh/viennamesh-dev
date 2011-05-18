/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GML_INTERFACE_GSSE_READDOMAIN
#define GML_INTERFACE_GSSE_READDOMAIN
//
// ===================================================================================
//
// *** GSSE includes
//
#include <gsse/datastructure/domain.hpp>
//
// *** SYSTEM includes
//
#include <string>
//
// ===================================================================================
//
//#define READERDEBUG
//
// ===================================================================================
//
namespace gml {

template < typename Domain >
void read_domain(Domain & domain, std::string filename)
{
#ifdef READERDEBUG
   std::cout << "reading domain .. " << std::endl;
#endif
   using namespace gsse::access_specifier; 

   typedef gsse::property_domain<Domain>                                                  GSSE_Domain;
   typedef typename gsse::result_of::property_SpaceTopologySegmentsT<GSSE_Domain>::type   SpaceTopologySegmentsT;
   typedef typename gsse::result_of::property_SpaceQuantitySegmentsT<GSSE_Domain>::type   SpaceQuantitySegmentsT;
   typedef typename gsse::result_of::property_FBPosT<GSSE_Domain>::type                   FBPosT; 
   typedef typename gsse::result_of::property_CoordPosT<GSSE_Domain>::type                CoordPosT;      
   typedef typename gsse::result_of::at_dim_index<SpaceTopologySegmentsT>::type           CellComplex;       
   typedef typename gsse::result_of::at_dim_index<SpaceQuantitySegmentsT>::type           QuanComplex;  
   typedef typename gsse::result_of::property_QuanCL<GSSE_Domain>::type                   StorageForm;
   typedef typename gsse::result_of::at_dim_isd<CellComplex, AT_cl>::type                 Cell;    

   SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(domain);
   SpaceQuantitySegmentsT& segments_quantity = gsse::at_dim<AQ>(domain);
   FBPosT&                 geometry          = gsse::at_dim<AP>(domain);

   // -----------------------------------
   std::ifstream file(filename.c_str(), std::ios::binary);
   // -----------------------------------
   std::string tempinput;
   long gsse_dim;
   file >> gsse_dim; // dimension

   long size_vertices;
   file >> size_vertices;

#ifdef READERDEBUG
   std::cout << "   reading geometry .." << std::endl;
#endif

   gsse::resize( size_vertices )( geometry );

   for (long i =0 ; i < size_vertices; ++i)
   {
      for (long di = 0; di < gsse_dim; ++di)
      {
         file >> geometry[i][di];
      }
   }
   // -----------------------------------   
   long size_segments;
   file >> size_segments;
   gsse::resize(size_segments)(segments_topology);
   gsse::resize(size_segments)(segments_quantity);
   // -----------------------------------  
   std::string name_segments;
   file >> tempinput;
#ifdef READERDEBUG
   std::cout << "   reading topology .." << std::endl;
#endif

   for (long cnt_seg = 0; cnt_seg < size_segments; ++cnt_seg)
   {
   #ifdef READERDEBUG
      std::cout << "      seg: " << cnt_seg << std::endl;
   #endif
   
      name_segments = tempinput;

      gsse::at_index(gsse::at(cnt_seg)(segments_topology)) = name_segments;
      gsse::at_index(gsse::at(cnt_seg)(segments_quantity)) = name_segments;   
      
      long size_segment_cells;
      file >> size_segment_cells;
      file >> tempinput;  // nb of different vertices

   #ifdef READERDEBUG
      std::cout << "      cells: " << std::endl;
   #endif
      
      for (long i = 0 ; i < size_segment_cells; ++i)
      {
         Cell cell;
         file >> tempinput; // cell handle/id
         
         for (unsigned int ci = 0; ci < cell.size(); ++ci)
         {
            file >> cell[ci];
         }
         gsse::at(i)(gsse::at_dim<AT_cl>(  gsse::at_fiber(gsse::at(cnt_seg)(segments_topology)))) = cell;
      }    
      // ### start with quantity complex
      //
   #ifdef READERDEBUG
      std::cout << "      quantities: " << std::endl;
   #endif      
      file >> tempinput; // quantity:
      if( tempinput == "Quantity:" )
      {
         while(1)
         {
            std::string quan_dimension;
            file >> quan_dimension; // vertex, edge, sheaf_vertex ..
            long nb_quantities;
            file >> nb_quantities; 
            
            for (long nb_q = 0; nb_q < nb_quantities; ++nb_q)
            {
               std::string quan_name;
               file >> quan_name;
               file >> tempinput;  // "type" 
               long x;
               file >> x; // in this version .. its always one ...
               long y;
               file >> y; // vector data .. 
               long quan_size;
               file >> quan_size;
               
               for (long i = 0; i < quan_size; ++i)
               {
                  long object_handle;
                  file >> object_handle;
      
                  if (quan_dimension == "sheaf_vertex")
                  {
                     for( long vec_i = 0; vec_i < y; vec_i++ )
                     {
                        double data;
                        file >> data;
                        gsse::at(vec_i)(gsse::at(object_handle)( 
                           gsse::at(quan_name)(gsse::at_dim<AT_vx>(gsse::at_fiber(gsse::at(cnt_seg)(segments_quantity))))  
                        )) = data;
                        //gsse::at(vec_i)(gsse::at(object_handle)( gml::vertex_quan( domain, cnt_seg, quan_name ) ) ) = data;
                     }
                  }
               }
            }
            file >> tempinput; // quantity:
            if (file.eof()) break;
            
            if (tempinput != "Quantity:")
            {
               break;
            }
         }
      }        
   }
        
}
/*

template<typename DomainT>
void read_domain(DomainT& domain, std::string filename_for_gssev01)
{
   using namespace gsse::access_specifier; 

   typedef gsse::property_domain<DomainT>                                                 GSSE_Domain;
   typedef typename gsse::result_of::property_SpaceTopologySegmentsT<GSSE_Domain>::type   SpaceTopologySegmentsT;
   typedef typename gsse::result_of::property_SpaceQuantitySegmentsT<GSSE_Domain>::type   SpaceQuantitySegmentsT;
   typedef typename gsse::result_of::property_FBPosT<GSSE_Domain>::type                   FBPosT; 
   typedef typename gsse::result_of::property_CoordPosT<GSSE_Domain>::type                CoordPosT;      
   typedef typename gsse::result_of::at_dim_index<SpaceTopologySegmentsT>::type           CellComplex;       
   typedef typename gsse::result_of::at_dim_index<SpaceQuantitySegmentsT>::type           QuanComplex;  
   typedef typename gsse::result_of::property_QuanCL<GSSE_Domain>::type                   StorageForm;

   SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(domain);
   SpaceQuantitySegmentsT& segments_quantity = gsse::at_dim<AQ>(domain);
   FBPosT&                 geometry          = gsse::at_dim<AP>(domain);

   std::ifstream file(filename_for_gssev01.c_str());
   std::string tempinput;
   long gsse_dim;
   file >> gsse_dim; // dimension

   long size_vertices;
   file >> size_vertices;
   
#ifdef READERDEBUG
   std::cout << "## read in coordinates: " << size_vertices << std::endl;
#endif
   for (long i =0 ; i < size_vertices; ++i)
   {
      CoordPosT coord;
      for (long di = 0; di < gsse_dim; ++di)
      {
         file >> coord[di];
      }
      gsse::at(i)(geometry) = coord;
   }
   
   long size_segments;
   file >> size_segments;
   gsse::resize(size_segments)(segments_topology);
   gsse::resize(size_segments)(segments_quantity);

#ifdef READERDEBUG   
   std::cout << "## read in segments: " << size_segments << std::endl;
#endif
//   long cnt_seg = 0;
   // ##############

   std::string name_segments;
   file >> tempinput;

   for (long cnt_seg = 0; cnt_seg < size_segments; ++cnt_seg)
   {
      name_segments = tempinput;

      gsse::at_index(gsse::at(cnt_seg)(segments_topology)) = name_segments;
      gsse::at_index(gsse::at(cnt_seg)(segments_quantity)) = name_segments;   // [RH][TODO][think about this] .. not necessary
      
      long size_segment_cells;
      file >> size_segment_cells;
   #ifdef READERDEBUG         
      std::cout << "\tcell size: " << size_segment_cells << std::endl;
   #endif
      file >> tempinput;  // nb of different vertices
      
      typedef typename gsse::result_of::at_dim_isd<CellComplex, AT_cl>::type CellT;  // ISD = 0
      for (long i = 0 ; i < size_segment_cells; ++i)
      {
         CellT cell;
         file >> tempinput; // cell handle/id
         
         for (unsigned int ci = 0; ci < cell.size(); ++ci)
         {
            file >> cell[ci];
         }
         gsse::pushback(gsse::at_dim<AT_cl>(  gsse::at_fiber(gsse::at(cnt_seg)(segments_topology)))) = cell;
      }
      // ### start with quantity complex
      //
      file >> tempinput; // quantity:
      if( tempinput == "Quantity:" )
      {
      #ifdef READERDEBUG   
         std::cout << "\tread in quantities .." << std::endl;
      #endif
         while(1)
         {
            std::string quan_dimension;
            file >> quan_dimension; // vertex, edge, ...
            long nb_quantities;
            file >> nb_quantities; 
            
            for (long nb_q = 0; nb_q < nb_quantities; ++nb_q)
            {
               std::string quan_name;
               file >> quan_name;
               long quan_size;
               file >> quan_size;
               
               for (long i = 0; i < quan_size; ++i)
               {
                  long object_handle;
                  file >> object_handle;
                  StorageForm quan;
                  //Storage_0form quan;
                  //file >> quan; // read-in simple 0-form
                  //file >> (0)(quan_name) (at_cl(complex))
               
                  //std::cout << " quandim: " << quan_dimension << "  quan_size: " << quan_size << "  quan: " << quan << std::endl;
      
                  if (quan_dimension == "vertex")
                  {
                     gsse::at(object_handle)(gsse::at(quan_name)(gsse::at_dim<AT_vx>(  gsse::at_fiber(gsse::at(cnt_seg)(segments_quantity))))) = quan;
                  }
                  else if (quan_dimension == "edge")
                  {
                     //gsse::at(object_handle)(gsse::at(quan_name)(gsse::at_dim<AT_ee>(  gsse::at_fiber(gsse::at(cnt_seg)(segments_quantity))))) = quan;
                  }
                  else if (quan_dimension == "facet")
                  {
                     //gsse::at(object_handle)(gsse::at(quan_name)(gsse::at_dim<AT_f0>(  gsse::at_fiber(gsse::at(cnt_seg)(segments_quantity))))) = quan;
                  }
                  else if (quan_dimension == "cell")
                  {
                     //gsse::at(object_handle)(gsse::at(quan_name)(gsse::at_dim<AT_cl>(  gsse::at_fiber(gsse::at(cnt_seg)(segments_quantity))))) = quan;
                  }
               }
            }
            file >> tempinput; // quantity:
            if (file.eof()) break;
            
            if (tempinput != "Quantity:")
            {
               break;
            }
         }
      }
   }

}
*/
} // end namespace: gml
#endif
























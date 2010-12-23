/* ============================================================================
   Copyright (c) 2007-2010 Franz Stimpfl                          franz@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GSSE_UTIL_FILE_FORMATS_GAU
#define GSSE_UTIL_FILE_FORMATS_GAU

#include "gsse/datastructure/domain.hpp"
#include "gsse/traversal.hpp"
#include "gsse/util.hpp"

// ============================================================================

//#define DEBUG_GAU

namespace gsse {
   namespace converter {   

      struct tag_gau {};
      
      struct gau_impl
      {
         template<typename FullSpace>
         void read(FullSpace& full_space, std::istream& input = std::cin)
         {
            using namespace gsse::access_specifier; 
            using namespace boost::phoenix;
            using namespace boost::phoenix::arg_names;
            // =======
            typedef typename gsse::result_of::at_dim<FullSpace, gsse::access_specifier::AC>::type       SpaceTopologySegmentsT;
            typedef typename gsse::result_of::at_dim<FullSpace, gsse::access_specifier::AQ>::type       SpaceQuantitySegmentsT;
            typedef typename gsse::result_of::at_dim<FullSpace, gsse::access_specifier::AP>::type       FBPosT;
            typedef typename gsse::result_of::at_dim_isd<FBPosT>::type                                  CoordPosT;
            
            typedef typename gsse::result_of::at_dim_index<SpaceTopologySegmentsT>::type                CellComplex;  
            typedef typename gsse::result_of::at_dim<CellComplex, gsse::access_specifier::AT_cl>::type  ContainerVXonCL;
            typedef typename gsse::result_of::val<ContainerVXonCL>::type                                CellT;
            
            SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(full_space);
            SpaceQuantitySegmentsT& segments_quantity = gsse::at_dim<AQ>(full_space);
            FBPosT&                 geometry          = gsse::at_dim<AP>(full_space);

            // =========================================

//          std::ifstream file(filename.c_str());
            std::string tempinput;

            size_t gsse_dim;
            input >> gsse_dim; // dimension

            //std::cout << "## reading geometric information .. " << std::endl;

            size_t size_vertices;
            input >> size_vertices;

            for (size_t i =0 ; i < size_vertices; ++i)
            {
               CoordPosT coord;
               for (size_t di = 0; di < gsse_dim; ++di)
               {
                  input >> coord[di];
               }
               gsse::at(i)(geometry) = coord;
            }

            //std::cout << "## reading topological information .." << std::endl;

   
            size_t size_segments;
            input >> size_segments;

#ifdef DEBUG_GAU
            std::cout << ".. segment size: " << size_segments << std::endl;
#endif

            gsse::resize(size_segments)(segments_topology);
            gsse::resize(size_segments)(segments_quantity);
   
            input >> tempinput;

            for(size_t cnt_seg = 0; cnt_seg < size_segments; ++cnt_seg)
            {
               if(tempinput == "Segment:")
                  input >> tempinput;
     
#ifdef DEBUG_GAU
               std::cout << "name seg: " << tempinput << std::endl;
#endif
     
               gsse::at_index(gsse::at(cnt_seg)(segments_topology)) = tempinput;
               gsse::at_index(gsse::at(cnt_seg)(segments_quantity)) = tempinput;
          
               size_t size_segment_cells;
               input >> size_segment_cells;
               input >> tempinput;  // number of segment vertices

#ifdef DEBUG_GAU
               std::cout << "..size_segment_cells: " << size_segment_cells << std::endl;
               std::cout << "..tempinput         : " << tempinput << std::endl;
#endif
     
               //      typedef typename gsse::result_of::at_dim_isd<CellComplex, AT_cl>::type CellT;
               for (size_t i = 0 ; i < size_segment_cells; ++i)
               {
                  CellT cell;
                  input >> tempinput; // cell handle/id
        
                  for (size_t ci = 0; ci < gsse::size(cell); ++ci)
                  {
                     input >> cell[ci];
                  }
                  gsse::pushback(gsse::at_dim<AT_cl>(  gsse::at_fiber(gsse::at(cnt_seg)(segments_topology)))) = cell;
        
#ifdef DEBUG_GAU
                  std::cout << ".. cell: " << cell << std::endl;
#endif	 
               }

#ifdef DEBUG_GAU
               std::cout << ".. topology read" << std::endl;
#endif
     
               input >> tempinput; // quantity: or segment name
               while(1)
               {
                  //std::cout << "initial tempinput: " << tempinput << std::endl;
                  if( tempinput == "Quantity:" )
                  {
                     std::string quan_dimension;
                     input >> quan_dimension; // vertex, edge, sheaf_vertex ..
                     //std::cout << "quan type: " << quan_dimension << std::endl;
                     long nb_quantities;
                     input >> nb_quantities; 
                     //std::cout << "number of quants: " << nb_quantities << std::endl;
                     for (long nb_q = 0; nb_q < nb_quantities; ++nb_q)
                     {
                        std::string quan_name;
                        input >> quan_name;
                        //std::cout << "  quanname: " << quan_name << std::endl;
                        input >> tempinput;  // "type" 
                        long x;
                        input >> x; // in this version .. its always one ...
                        long y;
                        input >> y; // vector data .. 
                        long quan_size;
                        input >> quan_size;
                        
                        for (long i = 0; i < quan_size; ++i)
                        {
                           long object_handle;
                           input >> object_handle;
               
                           if (quan_dimension == "sheaf_vertex")
                           {
                              for( long vec_i = 0; vec_i < y; vec_i++ )
                              {
                                 double data;
                                 input >> data;
                                 gsse::at(vec_i)(gsse::at(object_handle)( 
                                    gsse::at(quan_name)(gsse::at_dim<AT_vx>(gsse::at_fiber(gsse::at(cnt_seg)(segments_quantity))))  
                                 )) = data;
                              }
                           }
                           else
                           if (quan_dimension == "sheaf_edge")
                           {
                              for( long vec_i = 0; vec_i < y; vec_i++ )
                              {
                                 double data;
                                 input >> data;
                                 gsse::at(vec_i)(gsse::at(object_handle)( 
                                    gsse::at(quan_name)(gsse::at_dim<AT_ee>(gsse::at_fiber(gsse::at(cnt_seg)(segments_quantity))))  
                                 )) = data;
                              }
                           }
                           else
                           if (quan_dimension == "sheaf_cell")
                           {
                              for( long vec_i = 0; vec_i < y; vec_i++ )
                              {
                                 double data;
                                 input >> data;
                                 gsse::at(vec_i)(gsse::at(object_handle)( 
                                    gsse::at(quan_name)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(cnt_seg)(segments_quantity))))  
                                 )) = data;
                              }
                           }
                           else { //not recognized ... 
                           }
                        }
                     }
                     input >> tempinput; // quantity: or Segment name
                  }
                  if (input.eof()) break;
                  
                  if (tempinput != "Quantity:")  // remove me?
                  {
                     break;
                  }
               }        



               // [INFO] the quantity section is commented, because of backward compatibility     
               /*     
               // ### start with quantity complex
               //
               file >> tempinput; // quantity:

               #ifdef DEBUG_GAU
               std::cout << "..tempinput: " << tempinput << std::endl;
               #endif

               while(1)
               {
               std::string quan_dimension;
               file >> quan_dimension; // vertex, edge, ...
               size_t nb_quantities;
               file >> nb_quantities; 
	 
               for (size_t nb_q = 0; nb_q < nb_quantities; ++nb_q)
               {
               std::string quan_name;
               file >> quan_name;
               size_t quan_size;
               file >> quan_size;
	     
               for (size_t i = 0; i < quan_size; ++i)
               {
               size_t object_handle;
               file >> object_handle;
               Storage_0form quan;
               gsse::resize(1)(quan);
               file >> quan[0]; // read-in simple 0-form
		 
               #ifdef DEBUG_GAU
               std::cout << " quandim: " << quan_dimension << "  quan_size: " << quan_size << "  quan: " << quan[0] << std::endl;
               #endif
		 
               if (quan_dimension == "vertex")
               {
               gsse::at(object_handle)
               (gsse::at(quan_name)(gsse::at_dim<AT_vx>(gsse::at_fiber(gsse::at(cnt_seg)(segments_quantity))))) = quan;
               }
               else if (quan_dimension == "edge")
               {
               gsse::at(object_handle)
               (gsse::at(quan_name)(gsse::at_dim<AT_ee>(gsse::at_fiber(gsse::at(cnt_seg)(segments_quantity))))) = quan;
               }
               else if (quan_dimension == "facet")
               {
               gsse::at(object_handle)
               (gsse::at(quan_name)(gsse::at_dim<AT_f0>(gsse::at_fiber(gsse::at(cnt_seg)(segments_quantity))))) = quan;
               }
               else if (quan_dimension == "cell")
               {
               gsse::at(object_handle)
               (gsse::at(quan_name)(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(cnt_seg)(segments_quantity))))) = quan;
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
               */
            }
         }

         template<typename FullSpace>
         void write(FullSpace& full_space, std::ostream& output = std::cout)
         {
            using namespace gsse::access_specifier; 
            using namespace boost::phoenix;
            using namespace boost::phoenix::arg_names;
            
            // ==========

            typedef gsse::property_domain<FullSpace>  GSSE_Domain;
            typedef typename gsse::result_of::property_CellT<GSSE_Domain>::type                  CellT;
            typedef typename gsse::result_of::property_SpaceTopologySegmentsT<GSSE_Domain>::type SpaceTopologySegmentsT;

            static const size_t DIMG = gsse::result_of::property_DIMG<GSSE_Domain>::value;   
//          static const size_t DIMT = gsse::result_of::property_DIMT<GSSE_Domain>::value;   

            // ==========

            SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(full_space);   	

#ifdef DEBUG_GAU
            std::cout << ".. DIMG: " << DIMG <<  std::endl;
#endif   
   
            // geometric information
            //
            output << DIMG << std::endl;
            output << gsse::size(gsse::at_dim<AP>(full_space)) << std::endl;
            output.setf( std::ios::right,std::ios::adjustfield);
            output << std::setprecision(12) << std::setiosflags(std::ios::fixed);
   
            gsse::traverse<AP>()
               [
                  gsse::traverse()
                  [
                     output  << boost::phoenix::arg_names::_1 << val("  ")
                  ]
                  , output << val("") << std::endl
               ](full_space);

  
            // print the segment size
            //
            output << gsse::size(gsse::at_dim<AC>(full_space)) << std::endl;
   
            for (size_t cnt_seg = 0; cnt_seg < gsse::size(gsse::at_dim<AC>(full_space)); ++cnt_seg)
            {      
               size_t cellsize = gsse::size(gsse::at_dim<AT_cl>( gsse::at_fiber(gsse::at(cnt_seg) (segments_topology))));
               // [TODO] how can the segments vertex size be extracted ???
               //        the current version is not working ...
               //
               //      size_t vertexsize = gsse::at_dim<AP>(gsse::at(cnt_seg)(segments_topology)).size();
               //      size_t vertexsize = gsse::at_dim<AP>(full_space)[cnt_seg].size();
               size_t vertexsize = 0;

#ifdef DEBUG_GAU
               std::cout << "..cellsize: " << cellsize << " :: vertexsize: " << vertexsize << std::endl;
#endif
      
               // segment name   
               //
               std::string segname = gsse::at_index(gsse::at(cnt_seg)(segments_topology));

               if(segname == "") 
                  output << "Default-Segment-"+boost::lexical_cast<std::string>(cnt_seg) << std::endl;
               else
                  output << segname << std::endl;
      
               // print size of cells and vertices
               //
               output << cellsize << std::endl;   
               output << vertexsize << std::endl;      

               for (size_t ci = 0 ; ci < cellsize; ++ci)
               {
                  CellT cell = gsse::at_cl(ci) ( segments_topology[cnt_seg]);
                  output << ci;

#ifdef DEBUG_GAU
                  std::cout << ".. cell size: " << gsse::size(cell) << std::endl;
#endif
                  for (size_t voci = 0; voci < gsse::size(cell); ++voci)
                  {
                     output << " " << cell[voci];
                  }         
                  output << std::endl;         
               }
      
#ifdef DEBUG_GAU
               std::cout << ".. finished reading segments topology.." << std::endl;
#endif      
      
               // can also be formulated using phoenix
               //
               //       size_t cnt_cl = 0;
               //       gsse::traverse<AC>()
               //       [
               //          gsse::traverse<AT_cl>()
               //          [
               //             output << ref(cnt_cl) << val(" \t")
               //             ,
               //             ++ref(cnt_cl)
               //             ,
               //             gsse::traverse()
               //             [
               // 	     output << _1 << val("  ")
               //             ]
               //             , output << val("") << std::endl
               //          ]
               //       ](full_space);
      

               // [INFO] the quantity section is commented, because of backward compatibility

               // [INFO] write the vertex quantities
               //
               //       output << "Quantity: " << "vertex " << 
               // 	gsse::size(gsse::at_dim<AT_vx>(gsse::at_fiber(gsse::at_dim<AQ>(full_space)[cnt_seg]))) << std::endl;

               //       gsse::traverse<AQ>()
               // 	[
               //          gsse::traverse<AT_vx>()
               //          [
               // 	  output << val("") << gsse::acc0 << std::endl   // ## quantity name
               // 	  ,
               // 	  output << gsse::asize1 << std::endl
               // 	  ,
               // 	  gsse::traverse()
               // 	  [
               // 	   output << gsse::acc << val(" ")
               // 	   ,
               // 	   gsse::traverse() // for multiple values
               // 	   [
               // 	    output << boost::phoenix::arg_names::_1
               // 	   ]
               // 	   ,
               // 	   output << val("") << std::endl
               //           ]
               // 	 ]
               // 	](full_space);
               // 	](gsse::at_fiber(gsse::at(0)(segments_topology)));


               // [INFO] write the edge quantities
               //
               //       output << "Quantity: " << "edge " << 
               //          gsse::at_dim<AT_ee>(gsse::at_fiber(gsse::at_dim<AQ>(full_space)[cnt_seg])).size() << std::endl;


               //       gsse::traverse<AQ>()
               //       [
               //          gsse::traverse<AT_ee>()
               //          [
               //             output << val("") << gsse::acc << std::endl
               //             ,
               //             output << gsse::asize1 << std::endl
               //             ,
               //             gsse::traverse()
               //             [
               //                output << gsse::acc << val(" ")
               //                ,
               //                gsse::traverse()
               //                [
               //                   output << boost::phoenix::arg_names::_1
               //                ]
               //                ,
               //                output << val("") << std::endl
               //                ]
               //                ]
               //                ](full_space);
   

               // [INFO] write the facet quantities
               //
               //       output << "Quantity: " << "facet " << 
               //          gsse::at_dim<AT_f0>(gsse::at_fiber(gsse::at_dim<AQ>(full_space)[cnt_seg])).size() << std::endl;

               //         gsse::traverse<AQ>()
               //         [
               //         gsse::traverse<AT_f0>()
               //         [
               //         output << val("") << gsse::acc << std::endl
               //         ,
               //         output << gsse::asize1 << std::endl
               //         ,
               //         gsse::traverse()
               //         [
               //         output << gsse::acc << val(" ")
               //         ,
               //         gsse::traverse()
               //         [
               //         output << boost::phoenix::arg_names::_1
               //         ]
               //         ,
               //         output << val("") << std::endl
               //         ]
               //         ]
               //         ](full_space);

	
               // [INFO] write the cell quantities
               //
               //       output << "Quantity: " << "cell " << 
               //          gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at_dim<AQ>(full_space)[cnt_seg])).size() << std::endl;
      
               //         gsse::traverse<AQ>()
               //         [
               //         gsse::traverse<AT_cl>()
               //         [
               //         output << val("") << gsse::acc << std::endl
               //         ,
               //         output << gsse::asize1 << std::endl
               //         ,
               //         gsse::traverse()
               //         [
               //         output << gsse::acc << val(" ")
               //         ,
               //         gsse::traverse()
               //         [
               //         output << boost::phoenix::arg_names::_1
               //         ]
               //         ,
               //         output << val("") << std::endl
               //         ]
               //         ]
               //         ](full_space);
      

#ifdef DEBUG_GAU
               std::cout << ".. finished reading quantities .." << std::endl;
#endif      
            }
         }                  
      };   
   
   } // end namespace: converter
} // end namespace: gsse

#endif

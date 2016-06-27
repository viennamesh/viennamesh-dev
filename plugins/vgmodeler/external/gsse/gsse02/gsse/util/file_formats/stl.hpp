/* ============================================================================
   Copyright (c) 2007-2010 Franz Stimpfl                          franz@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GSSE_UTIL_FILE_FORMATS_STL
#define GSSE_UTIL_FILE_FORMATS_STL

#include "gsse/datastructure/domain.hpp"
#include "gsse/traversal.hpp"
#include "gsse/util.hpp"

// ============================================================================

namespace gsse {
   namespace converter {
      
      struct tag_stl  {};
      
      struct stl_impl
      {            
         template<typename FullSpaceT>
         void read(FullSpaceT& full_space, std::istream& input = std::cin)
         {
            using namespace gsse::access_specifier; 
            
            // ==========
               
            typedef gsse::property_domain<FullSpaceT>  GSSE_Domain;
            
            typedef typename gsse::result_of::property_CoordPosT<GSSE_Domain>::type              CoordPosT;
            typedef typename gsse::result_of::property_CellT<GSSE_Domain>::type                  CellT;
  
            typedef typename gsse::result_of::property_SpaceTopologySegmentsT<GSSE_Domain>::type SpaceTopologySegmentsT;
            typedef typename gsse::result_of::property_SpaceQuantitySegmentsT<GSSE_Domain>::type SpaceQuantitySegmentsT;
            typedef typename gsse::result_of::property_FBPosT<GSSE_Domain>::type                 FBPosT;
      
            typedef typename gsse::result_of::property_QuanCL<GSSE_Domain>::type                 StorageForm_cl;
      
            static const size_t DIMG = gsse::result_of::property_DIMG<GSSE_Domain>::value;   
            static const size_t DIMT = gsse::result_of::property_DIMT<GSSE_Domain>::value;   
  
            // ==========
  
            SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(full_space);
            SpaceQuantitySegmentsT& segments_quantity = gsse::at_dim<AQ>(full_space);
            FBPosT&                 geometry          = gsse::at_dim<AP>(full_space);
  
  
            std::string temp, name;
  
            // [INFO] STL does not store vertex indices direclty ->
            //        a geometrical points container has to be used 
            //
            std::map<CoordPosT, long>                     coordinate_map;
            typename std::map<CoordPosT, long>::iterator  cm_it;
            boost::array<long, 3>                       element_container;

            long vertex_counter  = 0;
            long cell_counter    = 0;
            long segment_counter = 0;

            input >> temp; // read "solid"
            input >> name;

            // [INFO] set the name of the segment - use 0, because only one segment is added
            //        stl has no information about segments
            //
            gsse::at_index(gsse::at(0)(segments_topology)) = name + "_0";  

            // [INFO] go through all facets
            //
            while(!input.eof() && !input.fail())
            {
               input >> temp; // read "facet"

               if(temp == "facet")
               {
                  input >> temp; // read "normal"
	  	  
                  StorageForm_cl normal_vector;
                  gsse::resize(DIMG) (normal_vector);
                  for(size_t dim = 0; dim < DIMG; ++dim)
                  {
                     input >> normal_vector[dim];
                  }
	  
                  input >> temp >> temp; // read "outer loop"

                  for(size_t element_index = 0; element_index < DIMT+1; ++element_index)   // DIMT + 1 -> simplex cell type
                  {
#ifdef DEBUG_STL
                     std::cout << "## element index: " << element_index << std::endl;
#endif
	      
                     input >> temp; // read "vertex"
          
                     // read the coordinates
                     //
                     CoordPosT new_point;
                     for(size_t j = 0; j < DIMG; ++j)
                     {
                        input >> new_point[j];
                     }

#ifdef DEBUG_STL
                     std::cout << "## new point: " << new_point << std::endl;
#endif

                     cm_it = coordinate_map.find(new_point);

                     if (cm_it ==  coordinate_map.end())
                     {
                        gsse::at(vertex_counter)(geometry) = new_point;

                        element_container[element_index] = vertex_counter;
                        coordinate_map[new_point]        = vertex_counter;
#ifdef DEBUG_STL
                        std::cout << ".. point not found: " << new_point << "/" << vertex_counter<< std::endl;
#endif
                        ++vertex_counter;
                     }
                     else
                     {
#ifdef DEBUG_STL
                        std::cout << " point found: " << (*cm_it).first << "/" << (*cm_it).second << std::endl;
#endif
                        element_container[element_index] = (*cm_it).second;
                     }	  	      
                  }
	  	  	  
                  // [INFO] add the topological element to the domain
                  //
                  CellT cell;	  
                  for (size_t ci = 0; ci < gsse::size(cell); ++ci)
                  {
                     cell[ci] = element_container[ci];
                  }
                  gsse::pushback(gsse::at_dim<AT_cl>(  gsse::at_fiber(gsse::at(segment_counter)(segments_topology))) ) = cell;

                  // store the normal vector in the FB
                  //
                  gsse::at(cell_counter)
                     (gsse::at("normal_vector")(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(segment_counter)(segments_quantity))))) = normal_vector;

	  	  	  	 
                  input >> temp; // read "endloop"
                  input >> temp; // read "endfacet"

                  cell_counter++;
               }
               else if(temp == "endsolid")
               {
                  break;
               }
            }

#ifdef DEBUG_STL
            std::cout << ".. number of vertices  inserted: " << vertex_counter << std::endl;
            std::cout << ".. number of triangles inserted: " << cell_counter << std::endl;
#endif
         }
    
         template<typename FullSpaceT>
         void write(FullSpaceT& full_space, std::ostream& output = std::cout)
         {
            using namespace gsse::access_specifier; 
            // ======
            typedef gsse::property_domain<FullSpaceT>  GSSE_Domain;

            typedef typename gsse::result_of::property_CoordPosT<GSSE_Domain>::type              CoordPosT;
            typedef typename gsse::result_of::property_CellT<GSSE_Domain>::type                  CellT;

            typedef typename gsse::result_of::property_SpaceTopologySegmentsT<GSSE_Domain>::type SpaceTopologySegmentsT;
            typedef typename gsse::result_of::property_FBPosT<GSSE_Domain>::type                 FBPosT;
            // ======
            SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(full_space);
            FBPosT&                 geometry          = gsse::at_dim<AP>(full_space);

            static const long DIMG = gsse::result_of::property_DIMG<GSSE_Domain>::value;  
   
#ifdef DEBUG_STL
            std::cout << ".. segments size: " << gsse::at_dim<AC>(full_space).size() << std::endl;
#endif

            output << std::setprecision(16) << std::setiosflags(std::ios::fixed);

            // [INFO] iterate over all segments
            //
            for(size_t si = 0; si < gsse::size(gsse::at_dim<AC>(full_space)); ++si)
            {
               output << "solid " << gsse::at_index(gsse::at_dim<AC>(full_space)[si]) << std::endl;
    
               // [INFO] iterate over all cells of the current segment
               //
               size_t cellsize = gsse::size(gsse::at_dim<AT_cl>( gsse::at_fiber((gsse::at(si) (segments_topology)))));
               for (size_t ci = 0 ; ci < cellsize; ++ci)
               {
                  CellT cell = gsse::at(ci) (gsse::at_dim<AT_cl>(gsse::at_fiber(segments_topology[si])));

                  // [TODO] until the normal vector is not specified in the quantity complex, a default normal vector is used
                  //
                  if(DIMG == 2)	  
                     output << "\tfacet normal 0.0 0.0" << std::endl;
                  else
                     output << "\tfacet normal 0.0 0.0 0.0" << std::endl;

                  output<< "\t\touter loop  " << std::endl;

                  // [INFO] iterate over the vertices of the cell and print them 
                  //
                  for (size_t voci = 0; voci < gsse::size(cell); ++voci)
                  {
                     CoordPosT vertex = gsse::at(cell[voci])(geometry);
                     output << "\t\t\tvertex ";
                     for(size_t coord = 0; coord < gsse::size(vertex); ++coord)
                        output << " " << vertex[coord];
                     output << std::endl;
                  }         
                  output << "\t\tendloop " << std::endl;
                  output << "\tendfacet " << std::endl;
               } 
            }
            output << "endsolid" << std::endl;            
         }
      };

   } // end namespace: converter
} // end namespace: gsse

#endif

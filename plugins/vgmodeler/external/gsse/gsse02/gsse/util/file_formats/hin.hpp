/* ============================================================================
   Copyright (c) 2007-2009 Franz Stimpfl                          franz@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GSSE_UTIL_FILE_FORMATS_HIN
#define GSSE_UTIL_FILE_FORMATS_HIN

#include "gsse/datastructure/domain.hpp"
#include "gsse/traversal.hpp"
#include "gsse/util.hpp"

// ** BOOST includes
#include<boost/lexical_cast.hpp>

// ============================================================================

namespace gsse {
   namespace converter {

      struct tag_hin  {};
      
      struct hin_impl
      {
         long parse_string(std::string& input)
         { 
            std::string::size_type pos1 = input.rfind("[")+1;
            std::string::size_type pos2 = input.size()-2;
            std::string temp2 = input.substr(pos1, pos2-pos1);
            
            long number = boost::lexical_cast<long>(temp2);
            
            std::cout << "[FS] .. in parse_number: " << number << std::endl;
            
            return number;
         }
         
         // ============================================================================
         // convert original hin file
         //
         template <typename FullSpaceT>
         void convert_hin_orig_2_gsse(const std::string& filename, FullSpaceT& full_space)
         {
            using namespace gsse::access_specifier; 

            typedef gsse::property_domain<FullSpaceT>  GSSE_Domain;

            typedef typename gsse::result_of::property_CoordPosT<GSSE_Domain>::type              CoordPosT;
            typedef typename gsse::result_of::property_CellT<GSSE_Domain>::type                  CellT;

            typedef typename gsse::result_of::property_SpaceTopologySegmentsT<GSSE_Domain>::type SpaceTopologySegmentsT;
            typedef typename gsse::result_of::property_SpaceQuantitySegmentsT<GSSE_Domain>::type SpaceQuantitySegmentsT;
            typedef typename gsse::result_of::property_FBPosT<GSSE_Domain>::type                 FBPosT;

            static const long DIMG = gsse::result_of::property_DIMG<GSSE_Domain>::value;   
   
            // ==========

            SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(full_space);
            SpaceQuantitySegmentsT& segments_quantity = gsse::at_dim<AQ>(full_space);
            FBPosT&                 geometry          = gsse::at_dim<AP>(full_space);

            std::cout << " ====================== " << std::endl;
            std::cout << " IMPORTER::  hin -> GSSE" << std::endl;
            std::cout << "  Trying filename: " << filename << std::endl;
            std::cout << " ====================== " << std::endl;
            std::cout << std::endl;
  
            // ----------------------------
  
            std::ifstream file;
            file.open(filename.c_str());
    
            std::vector<std::vector<long> > temp_storage;
  

            // geometric information
            //
            std::string temp;   
            file >> temp; // read "Vertices[12345]{"
            long number_of_points = parse_string(temp);

#ifdef DEBUG_HIN
            std::cout << "..temp: " << temp << std::endl;
            std::cout << "..number of points: " << number_of_points << std::endl; 
#endif  
  
            for (long vertex_counter = 0; vertex_counter < number_of_points; ++vertex_counter)
            {
               // read the coordinates
               //
               CoordPosT new_point;
               for(long j = 0; j < DIMG; ++j)
               {
                  file >> new_point[j];
               }
      
               gsse::at(vertex_counter)(geometry) = new_point;
            }

            file >> temp; // read: closing bracket }
  
  
            // topological information
            //
#ifdef DEBUG_HIN
            std::cout << "[FS] .. reading topological information" << std::endl;
#endif

            file >> temp; // read and parse "Polygons[12345]{"
            long number_of_polygons = parse_string(temp);

#ifdef DEBUG_HIN
            std::cout << ".. temp: " << temp << std::endl;  
            std::cout << ".. number of polygons: " << number_of_polygons << std::endl;
#endif

            for(long counter = 0; counter < number_of_polygons; ++counter)
            {     
               file >> temp; // read and parse: "Polygon[3]{"
               long number_of_polygon_points = parse_string(temp);

#ifdef DEBUG_HIN
               std::cout << ".. temp: " << temp << std::endl;
               std::cout << ".. number of polygons points: " << number_of_polygon_points << std::endl;
#endif
      
               std::vector<long> polygon;

               for(long counter2 = 0; counter2 < number_of_polygon_points; ++counter2)
               {
                  long point_id;
                  file >> point_id;
	  
                  polygon.push_back(point_id);  
               }
      
               temp_storage.push_back(polygon);
           
               file >> temp; // read: closing bracket }
            }

            file >> temp;   // read: closing bracket }

#ifdef DEBUG_HIN
            std::cout << "..temp_storage size: " << temp_storage.size() << std::endl;
#endif


            // read the segment information 
            // topological information is stored into the segment
            //   
            file >> temp; // read and parse: "Polyhedra[3]{"
            long number_of_segments = parse_string(temp);
   
#ifdef DEBUG_HIN
            std::cout << ".. temp: " << temp << std::endl;
            std::cout << ".. number of segments: " << number_of_segments << std::endl;
#endif
   
            for(long segment_counter = 0; segment_counter < number_of_segments; ++segment_counter)
            {
               file >> temp; // read and parse: "Polyhedron[12]{"
               long number_of_segment_polygons = parse_string(temp);
       
#ifdef DEBUG_HIN
               std::cout << ".. temp: " << temp << std::endl;      
#endif
       
               for(long segment_polygon_counter = 0; segment_polygon_counter < number_of_segment_polygons; ++segment_polygon_counter)
               {
                  long polygon_id;
                  file >> polygon_id;
	   
#ifdef DEBUG_HIN
                  std::cout << "..polygon_id: " << polygon_id << " :: segment_counter: " << segment_counter << " :: segment_poly_conter: " << segment_polygon_counter ;
#endif
	  
                  // [INFO] because the hin format stores orientation information in the sign
                  //     
                  if(polygon_id < 0) polygon_id *= -1;

                  // [INFO] HIN polygon id starts at 1 not at 0
                  //
                  polygon_id -= 1;

#ifdef DEBUG_HIN	  
                  std::cout << "..before add cell..:" << polygon_id << "\t";
                  std::cout << "..temp_storage: " 
                            << temp_storage[polygon_id][0] << " " 
                            << temp_storage[polygon_id][1] << " " 
                            << temp_storage[polygon_id][2] << "\t";
#endif

                  //  	  temp_segments[counter].add_cell_2(cell_2_vertex_mapping(temp_storage[polygon_id][0], 
                  // 								  temp_storage[polygon_id][1], 
                  // 								  temp_storage[polygon_id][2]));

                  CellT cell;	  
                  for (long ci = 0; ci < gsse::size(cell); ++ci)
                  {
                     cell[ci] = temp_storage[polygon_id][ci];
                  }
                  gsse::pushback(gsse::at_dim<AT_cl>(  gsse::at_fiber(gsse::at(segment_counter)(segments_topology))) ) = cell;
	  
                  std::string name_segment = "segment_" + segment_counter;
                  gsse::at_index(gsse::at(segment_counter)(segments_topology)) = name_segment;
                  gsse::at_index(gsse::at(segment_counter)(segments_quantity)) = name_segment;
	  
#ifdef DEBUG_HIN
                  std::cout << "..after add cell.." << std::endl;
#endif
               }
                  
               file >> temp; // read closing bracket }
            }
    
            file >> temp; // read closing bracket }

            file.close();

#ifdef DEBUG_HIN
            std::cout << ".. after read in of topological information" << std::endl;
#endif  
         }

         template <typename FullSpace>
         int convert_gsse_2_hin(FullSpace& full_space, std::ostream& output = std::cout)
         { 
            using namespace gsse::access_specifier; 
            using namespace boost::phoenix;
            using namespace boost::phoenix::arg_names;

            // ----------------------------

            typedef gsse::property_domain<FullSpace>  GSSE_Domain;
            typedef typename gsse::result_of::property_CellT<GSSE_Domain>::type                  CellT;
            typedef typename gsse::result_of::property_SpaceTopologySegmentsT<GSSE_Domain>::type SpaceTopologySegmentsT;

            SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(full_space);
  
            static const long DIMG = gsse::result_of::property_DIMG<GSSE_Domain>::value;     
  
            std::cout << " ====================== " << std::endl;
            std::cout << " EXPORTER::  GSSE -> Intel format" << std::endl;
            std::cout << "  Trying filename: " << std::endl;
            std::cout << " ====================== " << std::endl;
            std::cout << std::endl;
  
            // ----------------------------
  
            BOOST_STATIC_ASSERT(DIMG >= 2);
            BOOST_STATIC_ASSERT(DIMG <= 3);
   
            output << "Vertices[" 
                   << gsse::size(gsse::at_dim<AP>(full_space))
                   << "]{" << std::endl;
  
            output.setf(std::ios::right, std::ios::adjustfield);
            output << std::setprecision(16) << std::setiosflags(std::ios::fixed) 
                   <<  std::setiosflags(std::ios::showpos);
            // *** get all geometrical points
            //
            gsse::traverse<AP>()
               [
                  gsse::traverse()
                  [
                     output << boost::phoenix::arg_names::_1 << val("  ")
                  ]
                  , output << val("") << std::endl
               ](full_space);

            // *** end of the POINTS section
            output.unsetf(std::ios::showpos);
            output << "}" << std::endl;

#ifdef DEBUG_HIN
            std::cout << ".. segments size: " << gsse::size(segments_topology) << std::endl;
#endif    
  
            // calculate the total cell size
            //
            long cellsize = 0;
            for(long si = 0; si < gsse::size(segments_topology); ++si)
            {     
               gsse::traverse<AT_cl>()
                  [
                     ++ref(cellsize)
                  ](gsse::at_fiber(gsse::at(si)(segments_topology)));      
            }

            output << "Polygons[" << cellsize << "]{" << std::endl;


            for(long si = 0; si < gsse::size(segments_topology); ++si)
            {
               gsse::traverse<AT_cl>()
                  [
                     output << val("Polygon[") << gsse::asize1 << val("]{") << std::endl 
                     ,
                     gsse::traverse()
                     [
                        output << boost::phoenix::arg_names::_1 << val(" ")
                     ]
                     , 
                     output << val("") << std::endl 
                     ,
                     output << val("}") << std::endl
                  ](gsse::at_fiber(gsse::at(si)(segments_topology)));
            }
            output << "}" << std::endl;
  
      
            // *** begin of the SEGMENTS
            output << "Polyhedra[" << gsse::size(segments_topology) << "]{" << std::endl;

            for(long si = 0; si < gsse::size(segments_topology); ++si)
            {
               output << "Polyhedron[" 
                      << gsse::size(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(si)(segments_topology))))
                      << "]{" << std::endl;
      
               long cnt_cl = 1;
               gsse::traverse<AT_cl>()
                  [
                     output << ref(cnt_cl) << val(" ")
                     ,
                     ++ref(cnt_cl)
                  ](gsse::at_fiber(gsse::at(si)(segments_topology)));
      
               output << std::endl << "}" << std::endl;
            }
  
            output << "}" << std::endl;
    
            output << "Material["
                   << gsse::size(segments_topology)
                   << "]{" << std::endl;

            for(int i = 0; i < gsse::size(segments_topology); i++)
               output << i << " ";
  
            output << std::endl;
            output << "}" << std::endl;
    
            return 0;
         }
      };


      struct tag_hin2  {};
      
      struct hin2_impl
      {

         template <typename FullSpace>
         int convert_gsse_2_hin2(FullSpace& full_space, std::ostream& output = std::cout)
         {
            using namespace gsse::access_specifier; 
            using namespace boost::phoenix;
            using namespace boost::phoenix::arg_names;

            // ----------------------------

            typedef gsse::property_domain<FullSpace>  GSSE_Domain;
            typedef typename gsse::result_of::property_CellT<GSSE_Domain>::type                  CellT;
            typedef typename gsse::result_of::property_SpaceTopologySegmentsT<GSSE_Domain>::type SpaceTopologySegmentsT;

            SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(full_space);
  
            static const long DIMG = gsse::result_of::property_DIMG<GSSE_Domain>::value;    
  
            std::cout << " ====================== " << std::endl;
            std::cout << " EXPORTER::  GSSE -> Intel format" << std::endl;
            std::cout << "  Trying filename: " << std::endl;
            std::cout << " ====================== " << std::endl;
            std::cout << std::endl;

            // ----------------------------
  
            BOOST_STATIC_ASSERT(DIMG >= 2);
            BOOST_STATIC_ASSERT(DIMG <= 3);
  
            output << gsse::size(gsse::at_dim<AP>(full_space)) << std::endl;
            output.setf(std::ios::right, std::ios::adjustfield);
  

            // *** get all geometrical points
            // *** output all points to the wss output
            gsse::traverse<AP>()
               [
                  gsse::traverse()
                  [
                     output << std::setprecision(16) << std::setiosflags(std::ios::fixed) 
                     <<  std::setiosflags(std::ios::showpos)  << boost::phoenix::arg_names::_1 << val(" ")
                  ]
                  , output << val("") << std::endl
               ](full_space);


            // *** end of the POINTS section
            output.unsetf(std::ios::showpos);

 
            // *** begin of the SEGMENTS
            output << "# Number of segments " << std::endl;
            output << gsse::size(segments_topology) << std::endl; 
  
 
            // *** begin of the Topology (GRID)
            long seg_counter = 0;  
            for(long si = 0; si < gsse::size(segments_topology); ++si)
            {     
               gsse::traverse<AT_cl>()
                  [
                     output << ref(seg_counter) << val(" ")
                     ,
                     ++ref(seg_counter)
                     ,
                     gsse::traverse()
                     [
                        output << boost::phoenix::arg_names::_1 << val(" ")
                     ]
                     , 
                     output << val("") << std::endl 
                  ](gsse::at_fiber(gsse::at(si)(segments_topology)));
            }
      
            return 0;
         }


         template <typename FullSpaceT>
         int convert_hin2_2_gsse(const std::string& filename, FullSpaceT& full_space)
         {
            using namespace gsse::access_specifier; 

            typedef gsse::property_domain<FullSpaceT>  GSSE_Domain;

            typedef typename gsse::result_of::property_CoordPosT<GSSE_Domain>::type              CoordPosT;
            typedef typename gsse::result_of::property_CellT<GSSE_Domain>::type                  CellT;

            typedef typename gsse::result_of::property_SpaceTopologySegmentsT<GSSE_Domain>::type SpaceTopologySegmentsT;
            typedef typename gsse::result_of::property_SpaceQuantitySegmentsT<GSSE_Domain>::type SpaceQuantitySegmentsT;
            typedef typename gsse::result_of::property_FBPosT<GSSE_Domain>::type                 FBPosT;

            static const long DIMG = gsse::result_of::property_DIMG<GSSE_Domain>::value;   
   
            // ==========

            SpaceTopologySegmentsT& segments_topology = gsse::at_dim<AC>(full_space);
            SpaceQuantitySegmentsT& segments_quantity = gsse::at_dim<AQ>(full_space);
            FBPosT&                 geometry          = gsse::at_dim<AP>(full_space);

  
            std::cout << " ====================== " << std::endl;
            std::cout << " IMPORTER::  hin -> GSSE" << std::endl;
            std::cout << "  Trying filename: " << filename << std::endl;
            std::cout << " ====================== " << std::endl;
            std::cout << std::endl;
  
            // ----------------------------
  
            std::ifstream file;
            file.open( filename.c_str() );
    
   
            // geometry part 
            //
            std::string temp; 
            long number_of_points;
  
            file >> number_of_points;
  
#ifdef DEBUG_HIN
            std::cout << "[FS] .. num of points: " << number_of_points << std::endl;
#endif
  
            for (long vertex_counter = 0; vertex_counter < number_of_points; ++vertex_counter)
            {
               CoordPosT new_point;
               for(long j = 0; j < DIMG; ++j)
               {
                  file >> new_point[j];
               }
      
               gsse::at(vertex_counter)(geometry) = new_point;
      
#ifdef DEBUG_HIN
               std::cout << ".. new point: " << new_point << std::endl;
#endif
      
            }
   
            // topology part 
            // 
#ifdef DEBUG_HIN
            std::cout << ".. reading topological information" << std::endl;
#endif
  
            std::string tempstring;
            file >> tempstring >> tempstring >> tempstring >> tempstring;
  
            long size_segments;
            file >> size_segments; //  number of points 

#ifdef DEBUG_HIN
            std::cout << ".. segments size: " << size_segments << std::endl;
#endif
        
            long segment_id = -1;
            long tempindex;
  
            while (1)
            {	
               if(file >> tempindex) // read segment id
               {
#ifdef DEBUG_HIN
                  std::cout << ".. tempindex: " << tempindex << std::endl;
#endif

                  if(tempindex != segment_id)
                  {
                     segment_id = tempindex;
                     std::string name_segment = "segment_" + boost::lexical_cast<std::string>(segment_id);
                     gsse::at_index(gsse::at(segment_id)(segments_topology)) = name_segment;
                     gsse::at_index(gsse::at(segment_id)(segments_quantity)) = name_segment;		  
	      
#ifdef DEBUG_HIN
                     std::cout << ".. new segment_id: " << segment_id << std::endl;
#endif
                  }
	  
                  CellT cell;	  
                  for (long ci = 0; ci < gsse::size(cell); ++ci)
                  {
                     file >> cell[ci];
                  }
                  gsse::pushback(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(segment_id)(segments_topology))) ) = cell;
	  
#ifdef DEBUG_HIN
                  std::cout << ".. cell: " << cell << std::endl;
#endif
               }
               else
                  break;	  
            }
  
            file.close();
  
#ifdef DEBUG_HIN
            std::cout << ".. after read in of topological information" << std::endl;
#endif
    
            return 0;
         }
      };
   
   } // end namespace: converter
} // end namespace: gsse

#endif

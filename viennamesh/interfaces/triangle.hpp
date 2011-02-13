/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at
               Karl Rupp                                rupp@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */


#ifndef VIENNAMESH_INTERFACES_TRIANGLE_HPP
#define VIENNAMESH_INTERFACES_TRIANGLE_HPP

#include <vector>
#include <cstring>

#include <boost/array.hpp>
#include <boost/assert.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/static_assert.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/fusion/include/has_key.hpp>
#include <boost/fusion/include/at_key.hpp>

#include "viennamesh/methods.hpp"  
#include "viennamesh/interfaces/keys.hpp"

#include "viennautils/dumptype.hpp"

#define VOID void
#define ANSI_DECLARATORS
#define REAL double

extern "C"  { 
#include "triangle/triangle.h" 
}

//#define MESH_DEBUG
//#define MESH_ENGINE_DEBUG

namespace viennamesh {

template< typename NumericT >
struct triangle
{
   // -------------------------------------------------------------------------
   typedef NumericT  Numeric;
   typedef int       Index;

   static const int DIMG = 2;
   static const int DIMT = 2;
   static const int CELL_SIZE = DIMT+1;   // this holds true only for simplices
   
   typedef boost::array< Numeric , 2 >             point_type;
   typedef std::vector < point_type >              geometry_container_type;

   typedef boost::array< Index , 3 >               cell_type;
   typedef std::vector < cell_type >               topology_container_type;   
   typedef std::vector <topology_container_type>   segment_container_type;
   // -------------------------------------------------------------------------
   triangle()
   {
   #ifdef MESH_DEBUG
      std::cout << "MeshEngine: Triangle" << std::endl;
   #endif
      init(in);
      init(out);

      options = "";

      pointlist_index      = 0;
      segmentlist_index    = 0;
      regionlist_index     = 0;
      holelist_index       = 0;
      segment_index        = 0;
   }
   // -------------------------------------------------------------------------

   // -------------------------------------------------------------------------
   ~triangle()
   {
      freeMem(in);
      freeMem(out); 
   }
   // -------------------------------------------------------------------------

   // -------------------------------------------------------------------------
   void clear()
   {
      geometry_cont.clear();
      segment_cont.clear();
      freeMem(in); 
      freeMem(out);       
   }
   // -------------------------------------------------------------------------   

   // -------------------------------------------------------------------------   
   template<typename Map>
   void add(Map& map, 
            typename boost::enable_if< typename boost::fusion::result_of::has_key<Map, viennamesh::key::method>::type >::type* dummy = 0) 
   {
      add_method(boost::fusion::at_key<viennamesh::key::method>(map));
   }
   void add_method(viennamesh::method::constrained_delaunay const&)
   { 
      options = "zpq";
   }
   void add_method(viennamesh::method::conforming_delaunay const&)
   {
      options = "zpqD";
   }   
   void add_method(viennamesh::method::convex const&)
   {
      options = "z";
   }      
   // -------------------------------------------------------------------------    

   // -------------------------------------------------------------------------   
   template<typename Map>
   void add(Map& map, 
            typename boost::enable_if< typename boost::fusion::result_of::has_key<Map, viennamesh::key::size>::type >::type* dummy = 0) 
   {
      options.append("a" + boost::lexical_cast<std::string>(boost::fusion::at_key<viennamesh::key::size>(map)));
   }
   // -------------------------------------------------------------------------  

   // -------------------------------------------------------------------------   
   template<typename Map>
   void add(Map& map, 
            typename boost::enable_if< typename boost::fusion::result_of::has_key<Map, viennamesh::key::point>::type >::type* dummy = 0) 
   {
       extendPoints();
       in.pointlist[pointlist_index] = boost::fusion::at_key<viennamesh::key::point>(map)[0];
       pointlist_index++;
       in.pointlist[pointlist_index] = boost::fusion::at_key<viennamesh::key::point>(map)[1];
       pointlist_index++;  
   }
   // -------------------------------------------------------------------------      

   // -------------------------------------------------------------------------     
   template<typename Map>
   void add(Map& map, 
            typename boost::enable_if< typename boost::fusion::result_of::has_key<Map, viennamesh::key::constraint>::type >::type* dummy = 0) 
   {
      extendSegments();
      in.segmentlist[segmentlist_index] = boost::fusion::at_key<viennamesh::key::constraint>(map)[0];
      segmentlist_index++;
      in.segmentlist[segmentlist_index] = boost::fusion::at_key<viennamesh::key::constraint>(map)[1];
      segmentlist_index++;            
   }
   // -------------------------------------------------------------------------      

   // -------------------------------------------------------------------------     
   template<typename Map>
   void add(Map& map, 
            typename boost::enable_if< typename boost::fusion::result_of::has_key<Map, viennamesh::key::region>::type >::type* dummy = 0) 
   {
      // if this region method is called, add the A flag, 
      // so that Triangle distributes the regional quantities
      if(segment_index == 0) 
         options.append("A");
      extendRegions();
      in.regionlist[regionlist_index] = boost::fusion::at_key<viennamesh::key::region>(map)[0];
      regionlist_index++;
      in.regionlist[regionlist_index] = boost::fusion::at_key<viennamesh::key::region>(map)[1];
      regionlist_index++;
      in.regionlist[regionlist_index] = Numeric(segment_index);  // assign region id
      regionlist_index++;
      in.regionlist[regionlist_index] = Numeric(segment_index);  // assign region id
      regionlist_index++;
      segment_index++;
   }
   // -------------------------------------------------------------------------         

// gsse/gsse-v01-private/application/meshing/gsse_triangle/src/triangle++.hpp



   // -------------------------------------------------------------------------      
   void extendPoints()
   {
      if ( !in.pointlist)
         in.pointlist   = (Numeric *)malloc( 2 * sizeof(Numeric) );
      else   in.pointlist = (Numeric *)realloc ( in.pointlist, (in.numberofpoints+1) * 2 * sizeof(Numeric));
      in.numberofpoints++;
   }
   // -------------------------------------------------------------------------      

   // -------------------------------------------------------------------------      
   void extendSegments()
   {
      if ( !in.segmentlist)
         in.segmentlist   = (Index *)malloc( 2 * sizeof(Index) );
      else   in.segmentlist = (Index *)realloc ( in.segmentlist, (in.numberofsegments+1) * 2 * sizeof(Index));
      in.numberofsegments++;
   }
   // -------------------------------------------------------------------------      

   // -------------------------------------------------------------------------      
   void extendRegions()
   {
      if ( !in.regionlist)
         in.regionlist   = (Numeric *)malloc( 4 * sizeof(Numeric) );
      else   in.regionlist = (Numeric *)realloc ( in.regionlist, (in.numberofregions+1) * 4 * sizeof(Numeric));
      in.numberofregions++;
   }
   // -------------------------------------------------------------------------      

   // -------------------------------------------------------------------------   
   void operator()()
   {
      // default mesh property
      //
       if( options == "" )    
          this->add_method(viennamesh::method::constrained_delaunay());
   
   #ifndef MESH_ENGINE_DEBUG
      options.append("Q");
   #endif
   #ifdef MESH_DEBUG
      std::cout << "MeshEngine::Triangle: starting meshing process .." << std::endl;
      std::cout << "  parameter set:     " << options << std::endl;
      std::cout << "  input point size:  " << in.numberofpoints << std::endl;
      std::cout << "  input const size:  " << in.numberofsegments << std::endl;      
      std::cout << "  input region size: " << in.numberofregions << std::endl;            
   #endif   
      //char buffer[options.length()];
      char *buffer;
      buffer = (char *)malloc( options.length() * sizeof(char) );
      std::strcpy(buffer,options.c_str());

      BOOST_ASSERT(in.numberofpoints != 0);
      //BOOST_ASSERT(in.numberofsegments != 0);      

      triangulate(buffer, &in, &out, 0);
      
      if ( !out.pointlist)      
         std::cout << "\t::ERROR: pointlist" << std::endl;
      if ( !out.trianglelist)   
         std::cout << "\t::ERROR: trianglelist " << std::endl;

      //
      // extracting geometry data
      //
      geometry_cont.resize(out.numberofpoints);
      for(Index pnt_index = 0; pnt_index < out.numberofpoints; ++pnt_index)
      {
         Index index = pnt_index * 2;
         
         point_type pnt;
         pnt[0] = out.pointlist[index];
         pnt[1] = out.pointlist[index+1];
         
         geometry_cont[pnt_index] = pnt;
      }      
      //
      // extracting cell complex
      //
      size_t seg_id = 0;
      if(segment_index == 0) segment_index = 1;
      segment_cont.resize(segment_index);
      for(Index tri_index = 0; tri_index < out.numberoftriangles; ++tri_index)
      {
         Index index = tri_index * 3; 

         cell_type cell;
         cell[0] = out.trianglelist[index];
         cell[1] = out.trianglelist[index+1];
         cell[2] = out.trianglelist[index+2];         
         
         // only access triangle attributes if there are any
         // otherwise we get ourselves a segfault
         // if, for example, there is only one region, 
         // there is no need to call "add<region>", therefore
         // we have to counter this case
         if(out.numberoftriangleattributes > 0)
            seg_id = out.triangleattributelist[tri_index];
         
         //std::cout << "tri: " << tri_index << " : " << cell[0] << " " << cell[1] << " " 
         //   << cell[2] << " attr: " << out.triangleattributelist[tri_index] << std::endl;
         segment_cont[seg_id].push_back(cell);
      }
      //
      // release internal mesher output container, as the data has been already 
      // retrieved
      //
      freeMem(out);   
   }
   // -------------------------------------------------------------------------     

   // -------------------------------------------------------------------------     
   inline geometry_container_type &
   geometry()
   {
      return geometry_cont;
   }         
   // -------------------------------------------------------------------------     
   inline segment_container_type &
   topology()
   {
      return segment_cont;
   }       
   // -------------------------------------------------------------------------   

   // -------------------------------------------------------------------------
   void init(triangulateio& io)
   {
      io.pointlist = io.pointattributelist = 0;
      io.pointmarkerlist   = 0;
      io.numberofpoints = io.numberofpointattributes = 0;
      io.trianglelist = 0;
      io.triangleattributelist = io.trianglearealist = 0;
      io.neighborlist = 0;
      io.numberoftriangles = io.numberofcorners = 0;
      io.numberoftriangleattributes = 0;
      io.segmentlist = io.segmentmarkerlist = 0;
      io.numberofsegments = 0;
      io.holelist = 0; 
      io.numberofholes = 0;
      io.regionlist = 0;
      io.numberofregions = 0;
      io.edgelist = io.edgemarkerlist = 0;
      io.normlist = 0;
      io.numberofedges = 0;
   }   
   // -------------------------------------------------------------------------   
   
   // -------------------------------------------------------------------------  
   void freeMem(triangulateio& io)
   {
      if (io.pointlist != NULL)
      {  
         free (io.pointlist);
         io.pointlist = NULL;
      }
      //if (io.holelist != NULL) // [JW] segfaults O.o
      //{  
      //   free (io.holelist);
      //   io.holelist = NULL;
      //}      
      if (io.pointattributelist != NULL)
      {
         free (io.pointattributelist);
         io.pointattributelist = NULL;
      }
      if (io.pointmarkerlist != NULL)
      {
         free (io.pointmarkerlist);
         io.pointmarkerlist = NULL;
      }
      if (io.trianglelist != NULL)
      {
         free (io.trianglelist);
         io.trianglelist = NULL;
      }
      if (io.triangleattributelist != NULL)   
      {
         free (io.triangleattributelist);
         io.triangleattributelist = NULL;
      }
      if (io.neighborlist != NULL)
      {
         free (io.neighborlist);
         io.neighborlist = NULL;
      }
      if (io.segmentlist != NULL)
      {
         free (io.segmentlist);
         io.segmentlist = NULL;
      }
      if (io.segmentmarkerlist != NULL)
      {
         free (io.segmentmarkerlist);
         io.segmentmarkerlist = NULL;
      }
      if (io.edgelist != NULL)               
      {
         free (io.edgelist);
         io.edgelist = NULL;
      }
      if (io.edgemarkerlist != NULL)        
      {
         free (io.edgemarkerlist);
         io.edgemarkerlist = NULL;
      }
      if (io.normlist != NULL)               
      {
         free (io.normlist);
         io.normlist = NULL;
      }
   }   
   // -------------------------------------------------------------------------     

   // -------------------------------------------------------------------------
   triangulateio  in, 
                  out;

   std::string    options;

   unsigned long  pointlist_index,
                  segmentlist_index,
                  regionlist_index,
                  holelist_index,
                  segment_index;   

   geometry_container_type      geometry_cont;      
   segment_container_type       segment_cont;
   // -------------------------------------------------------------------------   
};

} // end namespace viennamesh

#endif






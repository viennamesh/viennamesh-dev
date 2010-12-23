/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       
   Copyright (c) 2007-2009 Franz Stimpfl                          franz@gsse.at   

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GML_GENERATION_INTERFACES_TRIANGLE2D
#define GML_GENERATION_INTERFACES_TRIANGLE2D

//
// ===============================================================
//
// *** TRIANGLE2D defines
//
#define VOID void
#define ANSI_DECLARATORS
#define REAL double
//
// *** DEBUG defines
//
//#define MESH_DEBUG
//#define MESH_ENGINE_DEBUG
//
// ===============================================================
//
// *** SYSTEM includes
//
#include <cstring>
#include <vector>
//
// *** BOOST includes
//
#include <boost/assert.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/static_assert.hpp>
//
// *** GML includes
//
#include "../utils/property.hpp"
#include "../../util/common.hpp"
#include "../../util/debug.hpp"
#include "../../datastructure/metric_object.hpp"
#include "../../interface/print_gsse01.hpp"
#include "../../interface/print_ply.hpp"
//
// *** TRIANGLE2D includes
//
extern "C"  { 
#include <triangle/triangle.h> 
}
//
// ===============================================================
//
namespace gml {
// ===============================================================
template< typename NumericT >
struct Triangle2D
{
   // -------------------------------------------------------------------------
   typedef NumericT  Numeric;
   typedef int       Index;

   typedef gml::metric_object < Numeric , 2 >      Point;
   typedef std::vector < Point >                   ResultPointContainer;

   typedef gml::metric_object < Index , 3 >        Cell;
   typedef std::vector < Cell >                    ResultCellContainer;   
   // -------------------------------------------------------------------------
   Triangle2D()
   {
   #ifdef MESH_DEBUG
      std::cout << "GML::MeshEngine::Triangle2D::initiating .." << std::endl;
   #endif
      init(in);
      init(out);

      options = "";

      pointlist_index      = 0;
      segmentlist_index    = 0;
      regionlist_index     = 0;
      holelist_index       = 0;
   }
   // -------------------------------------------------------------------------
   ~Triangle2D()
   {
      freeMem(in);
      freeMem(out); 
   }
   // -------------------------------------------------------------------------
   void release()
   {
      point_cont_result.clear();
      cell_cont_result.clear();
      freeMem(in); 
      freeMem(out);       
   }
   // -------------------------------------------------------------------------   
   void add(gml::property::ConformingDelaunay const& ) 
   {  
   #ifdef MESH_DEBUG
      std::cout << "GML::MeshEngine::Triangle2D::SetProperty: Conforming Delaunay .." << std::endl;
   #endif   
      options = "zpqD";
   }   
   void add(gml::property::ConstrainedDelaunay const& ) 
   {  
   #ifdef MESH_DEBUG
      std::cout << "GML::MeshEngine::Triangle2D::SetProperty: Constrained Delaunay .." << std::endl;
   #endif      
      options = "zpq";
   }     
   void add(gml::property::Convex const& ) 
   {  
   #ifdef MESH_DEBUG
      std::cout << "GML::MeshEngine::Triangle2D::SetProperty: Convex .." << std::endl;
   #endif      
      options = "z";
   }      
   void add(gml::property::size const& size) // maximum area constraint
   {  
   #ifdef MESH_DEBUG
      std::cout << "GML::MeshEngine::Triangle2D::SetProperty: Size .." << std::endl;
   #endif      
      options.append("a" + boost::lexical_cast<std::string>(size._val));
   }      
   void add(gml::property::angle const& angle) 
   {  
   #ifdef MESH_DEBUG
      std::cout << "GML::MeshEngine::Triangle2D::SetProperty: Angle .." << std::endl;
   #endif     
      std::string::size_type pos = options.find("q",0);
      if( pos != std::string::npos )   options.erase( pos, 1 );      
      options.append("q" + boost::lexical_cast<std::string>(angle._val));
   }    
   void add(gml::property::radius_edge_ratio const& radius_edge_ratio) 
   {  
      std::cout << "GML::MeshEngine::Triangle2D::SetProperty: Minimum Radius-Edge Ratio " << std::endl;
      std::cout << " ## WARNING ## There is no radius-edge-ratio property available" << std::endl;
   }          
   // -------------------------------------------------------------------------   
   template<typename PointT>
   void add(PointT const& pnt,            // const-reference specialization
            typename boost::enable_if< 
               typename boost::is_floating_point<
                  typename gml::result_of::val< PointT >::type > >::type* dummy = 0) 
   {   
      extendPoints();

      in.pointlist[pointlist_index] = pnt[0];
      pointlist_index++;
      in.pointlist[pointlist_index] = pnt[1];
      pointlist_index++;   
   }
   template<typename PointT>
   void add(PointT & pnt,                 // reference specialization
            typename boost::enable_if< 
               typename boost::is_floating_point<
                  typename gml::result_of::val< PointT >::type > >::type* dummy = 0) 
   {
      extendPoints();

      in.pointlist[pointlist_index] = pnt[0];
      pointlist_index++;
      in.pointlist[pointlist_index] = pnt[1];
      pointlist_index++;   
   }   
   // -------------------------------------------------------------------------
   template <typename PointIndicesT>
   void add(PointIndicesT const& indices, // const-reference specialization
            typename boost::enable_if< 
               typename boost::is_integral<
                  typename gml::result_of::val< PointIndicesT >::type > >::type* dummy = 0) 
   {
      extendSegments();

      in.segmentlist[segmentlist_index] = indices[0];
      segmentlist_index++;
      in.segmentlist[segmentlist_index] = indices[1];
      segmentlist_index++;
   }
   template <typename PointIndicesT>
   void add(PointIndicesT & indices,      // reference specialization
            typename boost::enable_if< 
               typename boost::is_integral<
                  typename gml::result_of::val< PointIndicesT >::type > >::type* dummy = 0) 
   {
      extendSegments();

      in.segmentlist[segmentlist_index] = indices[0];
      segmentlist_index++;
      in.segmentlist[segmentlist_index] = indices[1];
      segmentlist_index++;
   }
   // -------------------------------------------------------------------------     
   template <typename PointT>
   void add_hole(PointT const& point)
   {
      extendHoles();

      in.holelist[holelist_index] = point[0];
      holelist_index++;
      in.holelist[holelist_index] = point[1];
      holelist_index++;   
   }
   template <typename PointT>
   void add_hole(PointT& point)
   {
      extendHoles();
      
      in.holelist[holelist_index] = point[0];
      holelist_index++;
      in.holelist[holelist_index] = point[1];
      holelist_index++;   
   }   
   // -------------------------------------------------------------------------   
   void start()
   {
      // default mesh property
      //
      if( options == "" )    (*this).add( gml::property::constrained_delaunay  );
   
   #ifndef MESH_ENGINE_DEBUG
      options.append("Q");
   #endif
   #ifdef MESH_DEBUG
      std::cout << "GML::MeshEngine::Triangle2D: starting meshing process .." << std::endl;
      std::cout << "  parameter set: " << options << std::endl;
   #endif   
      char buffer[options.length()];
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
      gsse::resize( out.numberofpoints )( point_cont_result );
      for(Index pnt_index = 0; pnt_index < out.numberofpoints; ++pnt_index)
      {
         Index index = pnt_index * 2;
         
         gsse::at( pnt_index )( point_cont_result ) = 
            Point( out.pointlist[index], out.pointlist[index+1] );
      }      
      //
      // extracting topology data
      //
      gsse::resize( out.numberoftriangles )( cell_cont_result );
      for(Index tri_index = 0; tri_index < out.numberoftriangles; ++tri_index)
      {
         Index index = tri_index * 3; 

         gsse::at( tri_index )( cell_cont_result ) = 
            Cell( out.trianglelist[index],
                  out.trianglelist[index+1],
                  out.trianglelist[index+2] );
      } 
      //
      // release internal mesher output container, as the data has been already 
      // retrieved
      //
      freeMem(out);   

   }
   // -------------------------------------------------------------------------     
   inline ResultPointContainer &
   geometry()
   {
      return point_cont_result;
   }         
   // -------------------------------------------------------------------------     
   inline ResultCellContainer &
   topology()
   {
      return cell_cont_result;
   }       
   // -------------------------------------------------------------------------   
   void print_input_ply(std::string filename)
   {
      typedef std::vector < Point >        Geometry;   
      
      Geometry in_geom;
      
      for(long pnt_index = 0; pnt_index < in.numberofpoints; ++pnt_index)
      {
         long index = pnt_index * 2;
      
         in_geom.push_back( Point( in.pointlist[index],
                                        in.pointlist[index+1] ) );
      }         
      typedef gml::metric_object < long , 2 >    EdgeT;
      typedef std::vector < EdgeT >             Topology;   
      
      Topology  in_topo;   
      
      for(long cell_index = 0; cell_index < in.numberofsegments; ++cell_index)
      {
         long index = cell_index * 2;
      
         in_topo.push_back( EdgeT( in.segmentlist[index],
                                   in.segmentlist[index+1] ) );
      }       

      std::cout << "debug: " << in.numberofholes << std::endl;
      Geometry holes; 
      for(long pnt_index = 0; pnt_index < in.numberofholes; ++pnt_index)
      {
         long index = pnt_index * 2;
         std::cout << "  " << in.holelist[index] << " " << in.holelist[index+1] << std::endl;
         holes.push_back( Point( in.holelist[index],
                                        in.holelist[index+1] ) );
      }   

      print_ply(in_topo, in_geom, holes, filename);          
   }
   // -------------------------------------------------------------------------   
   void print_input_gsse(std::string filename)
   {
      typedef std::vector < Point >        Geometry;   
      
      Geometry in_geom;
      
      for(long pnt_index = 0; pnt_index < in.numberofpoints; ++pnt_index)
      {
         long index = pnt_index * 2;
      
         in_geom.push_back( Point( in.pointlist[index],
                                        in.pointlist[index+1] ) );
      }         
      typedef gml::metric_object < long , 2 >    EdgeT;
      typedef std::vector < EdgeT >             Topology;   
      
      Topology  in_topo;   
      
      for(long cell_index = 0; cell_index < in.numberofsegments; ++cell_index)
      {
         long index = cell_index * 2;
      
         in_topo.push_back( EdgeT( in.segmentlist[index],
                                   in.segmentlist[index+1] ) );
      }  
      
      print_simple_gsse01(in_topo, in_geom, filename);   
   }   
   // -------------------------------------------------------------------------      
   void extendPoints()
   {
      if ( !in.pointlist)
         in.pointlist   = (Numeric *)malloc( 2 * sizeof(Numeric) );
      else   in.pointlist = (Numeric *)realloc ( in.pointlist, (in.numberofpoints+1) * 2 * sizeof(Numeric));
      in.numberofpoints++;
   }
   // -------------------------------------------------------------------------      
   void extendSegments()
   {
      if ( !in.segmentlist)
         in.segmentlist   = (Index *)malloc( 2 * sizeof(Index) );
      else   in.segmentlist = (Index *)realloc ( in.segmentlist, (in.numberofsegments+1) * 2 * sizeof(Index));
      in.numberofsegments++;
   }
   // -------------------------------------------------------------------------      
   void extendHoles()
   {
      if ( !in.holelist)
         in.holelist   = (Numeric *)malloc( 2 * sizeof(Numeric) );
      else   in.holelist = (Numeric *)realloc ( in.holelist, (in.numberofholes+1) * 2 * sizeof(Numeric));
      in.numberofholes++;
   }   
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
   triangulateio  in, 
                  out;

   std::string    options;

   unsigned long  pointlist_index,
                  segmentlist_index,
                  regionlist_index,
                  holelist_index;   

   ResultPointContainer       point_cont_result;      
   ResultCellContainer        cell_cont_result;                     
   // -------------------------------------------------------------------------   
};

} // end namespace: gml
#endif

/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       
   Copyright (c) 2007-2009 Franz Stimpfl                          franz@gsse.at   

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GML_GENERATION_INTERFACES_TETGEN
#define GML_GENERATION_INTERFACES_TETGEN
//
// ===============================================================
//
// *** TETGEN defines
//
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
//
// *** TETGEN includes
//
#include <tetgen/tetgen.h>
//
// ===============================================================
//
namespace gml {

template< typename NumericT >
struct Tetgen
{
   // -------------------------------------------------------------------------
   typedef NumericT  Numeric;
   typedef int       Index;

   typedef gml::metric_object < Numeric , 3 >      Point;
   typedef std::vector < Point >                   ResultPointContainer;

   typedef gml::metric_object < Index , 4 >        Cell;
   typedef std::vector < Cell >                    ResultCellContainer;   
   // -------------------------------------------------------------------------
   Tetgen()
   {
   #ifdef MESH_DEBUG
      std::cout << "GML::MeshEngine::Tetgen::initiating .." << std::endl;
   #endif
      options = "";

      init(in);
      init(out);

      pointlist_index               = 0;
      regionlist_index              = 0;
      mesher_facet_index            = 0;
   }
   // -------------------------------------------------------------------------
   ~Tetgen()
   {
      release_memory();
   }
   // -------------------------------------------------------------------------
   void add(gml::property::ConformingDelaunay const& ) 
   {  
   #ifdef MESH_DEBUG
      std::cout << "GML::MeshEngine::Tetgen::SetProperty: Conforming Delaunay .." << std::endl;
   #endif   
      options = "pqz";
   }   
   void add(gml::property::ConstrainedDelaunay const& ) 
   {  
   #ifdef MESH_DEBUG
      std::cout << "GML::MeshEngine::Tetgen::SetProperty: Constrained Delaunay .." << std::endl;
   #endif      
      options = "pz";
   }     
   void add(gml::property::Convex const& ) 
   {  
   #ifdef MESH_DEBUG
      std::cout << "GML::MeshEngine::Tetgen::SetProperty: Convex .." << std::endl;
   #endif      
      options = "z";
   }      
   void add(gml::property::size const& size) // maximum volume constraint
   {  
   #ifdef MESH_DEBUG
      std::cout << "GML::MeshEngine::Tetgen::SetProperty: Maximum Size .." << std::endl;
   #endif      
      options.append("a" + boost::lexical_cast<std::string>(size._val));
   }      
   void add(gml::property::radius_edge_ratio const& radius_edge_ratio) 
   {  
   #ifdef MESH_DEBUG
      std::cout << "GML::MeshEngine::Tetgen::SetProperty: Minimum Radius-Edge Ratio .." << std::endl;
   #endif      
      std::string::size_type pos = options.find("q");
      if( pos != std::string::npos )   options.erase( pos, 1 );  
      options.append("q" + boost::lexical_cast<std::string>(radius_edge_ratio._val));
   }       
   void add(gml::property::angle const& angle) 
   {  
      std::cout << "GML::MeshEngine::Tetgen::SetProperty: Angle " << std::endl;
      std::cout << " ## WARNING ## There is no angle property available" << std::endl;
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
      in.pointlist[pointlist_index] = pnt[2];
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
      in.pointlist[pointlist_index] = pnt[2];
      pointlist_index++;
   }   
   // -------------------------------------------------------------------------
   template <typename PointIndicesT>
   void add(PointIndicesT const& indices, // const-reference specialization
            typename boost::enable_if< 
               typename boost::is_integral<
                  typename gml::result_of::val< PointIndicesT >::type > >::type* dummy = 0) 
   {
      (*this).extendFacets();

      f = &in.facetlist[mesher_facet_index];
      f->numberofpolygons = 1;
      f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
      f->numberofholes = 0;
      f->holelist = NULL;
      p = &f->polygonlist[0];
      p->numberofvertices = 3;
      p->vertexlist = new int[p->numberofvertices];
      p->vertexlist[0] = indices[0];
      p->vertexlist[1] = indices[1];
      p->vertexlist[2] = indices[2];

      mesher_facet_index++;
   }
   template <typename PointIndicesT>
   void add(PointIndicesT & indices,      // reference specialization
            typename boost::enable_if< 
               typename boost::is_integral<
                  typename gml::result_of::val< PointIndicesT >::type > >::type* dummy = 0) 
   {
      (*this).extendFacets();
      f = &in.facetlist[mesher_facet_index];
      f->numberofpolygons = 1;

      f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
      f->numberofholes = 0;
      f->holelist = NULL;

      p = &f->polygonlist[0];
      p->numberofvertices = 3;
      p->vertexlist = new int[p->numberofvertices];
      p->vertexlist[0] = indices[0];
      p->vertexlist[1] = indices[1];
      p->vertexlist[2] = indices[2];

      mesher_facet_index++;
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
   void print_input(std::string filename)
   {
      typedef std::vector < Point >        Geometry;   
      
      Geometry in_geom;
      
      for(long pnt_index = 0; pnt_index < in.numberofpoints; ++pnt_index)
      {
         long index = pnt_index * 3;
      
         in_geom.push_back( Point( in.pointlist[index],
                                   in.pointlist[index+1],
                                   in.pointlist[index+2] ) );
      }         
      typedef gml::metric_object < long , 3 >    FaceT;
      typedef std::vector < FaceT >             Topology;   
      
      Topology  in_topo;   
      
      for(long cell_index = 0; cell_index < in.numberoffacets; ++cell_index)
      {
         f = &in.facetlist[cell_index];
         
         p = &f->polygonlist[0];
      
         in_topo.push_back( FaceT( p->vertexlist[0],
                                   p->vertexlist[1],
                                   p->vertexlist[2] ) );
      }         
      
      print_simple_gsse01(in_topo, in_geom, filename);   
   }      
   // -------------------------------------------------------------------------   
   void start()
   {
      //
      // default mesh property
      //
      if( options == "" )    (*this).add( gml::property::constrained_delaunay  );

   #ifndef MESH_ENGINE_DEBUG
      options.append("Q");
   #endif
   #ifdef MESH_DEBUG
      std::cout << "GML::MeshEngine::Tetgen: starting meshing process .." << std::endl;
      std::cout << "  parameter set: " << options << std::endl;
   #endif   
      char buffer[options.length()];
      std::strcpy(buffer,options.c_str());

      BOOST_ASSERT(in.numberofpoints != 0);
      //BOOST_ASSERT(in.numberoffacets != 0);      
      //
      // start meshing process
      //
      tetrahedralize(buffer, &in, &out);

      if ( !out.pointlist)      
         std::cout << "\t::ERROR: pointlist" << std::endl;

      //
      // extracting geometry data
      //
      gsse::resize( out.numberofpoints )( point_cont_result );
      for(Index pnt_index = 0; pnt_index < out.numberofpoints; ++pnt_index)
      {
         Index index = pnt_index * 3;
         
         gsse::at( pnt_index )( point_cont_result ) = 
            Point( out.pointlist[index], 
                   out.pointlist[index+1],
                   out.pointlist[index+2] );
      }      

      //
      // extracting topology data
      //
      gsse::resize( out.numberoftetrahedra )( cell_cont_result );
      for(Index tetr_index = 0; tetr_index < out.numberoftetrahedra; ++tetr_index)
      {
         Index index = tetr_index * 4; 

         gsse::at( tetr_index )( cell_cont_result ) = 
            Cell( out.tetrahedronlist[index],
                  out.tetrahedronlist[index+1],
                  out.tetrahedronlist[index+2],
                  out.tetrahedronlist[index+3] );
      }       

   }
   // -------------------------------------------------------------------------   
   void extendPoints()
   {
        if ( !in.pointlist)
          in.pointlist   = (Numeric *)malloc( 3 * sizeof(Numeric) );
      else   in.pointlist = (Numeric *)realloc ( in.pointlist, (in.numberofpoints+1) 
                            * 3 * sizeof(Numeric));
      in.numberofpoints++;
   }
   // -------------------------------------------------------------------------      
   void extendFacets()
   {
        if ( !in.facetlist)
          in.facetlist   = (tetgenio::facet *)malloc( sizeof(tetgenio::facet) );
      else   in.facetlist = (tetgenio::facet *)realloc ( in.facetlist, (in.numberoffacets+1) 
                            * sizeof(tetgenio::facet));
      in.numberoffacets++;
   }
   // -------------------------------------------------------------------------      
   void init(tetgenio& io)
   {
      io.numberofpoints = 0;
      io.numberoffacets = 0;
      io.numberofregions = 0;
   }
   // -------------------------------------------------------------------------     
   void release_memory()
   {
      for(Index facet_index = 0; facet_index <  in.numberoffacets; ++facet_index)
      {
         f = &in.facetlist[facet_index];    
         p = &f->polygonlist[0];

         delete[] p->vertexlist;
         delete[] f->polygonlist;  
      }      
      if(in.pointlist)           free(in.pointlist);
      if(in.facetlist)           free(in.facetlist);   
      
      in.pointlist = NULL;
      in.facetlist = NULL;
      
      delete[] out.pointmarkerlist;      
   } 
   // -------------------------------------------------------------------------      
   std::string    options;

   tetgenio          in, 
                     out;
   tetgenio::facet   *f;
   tetgenio::polygon *p;
   
   unsigned long  pointlist_index,
                  regionlist_index,
                  mesher_facet_index;
                  
   ResultPointContainer       point_cont_result;      
   ResultCellContainer        cell_cont_result;    
   // -------------------------------------------------------------------------   

};

} // end namespace: gml
#endif

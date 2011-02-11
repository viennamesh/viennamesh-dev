/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       
   Copyright (c) 2007-2009 Franz Stimpfl                          franz@gsse.at    

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GML_GENERATION_INTERFACES_CGAL   
#define GML_GENERATION_INTERFACES_CGAL
//
// ===================================================================================
//
// *** DEBUG defines
//
#define MESH_DEBUG
#define MESH_ENGINE_DEBUG
//
// ===================================================================================
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
#include "../../util/copy.hpp"
#include "../../util/debug.hpp"
#include "../../util/for_each.hpp"
#include "../../util/reference.hpp"
#include "../../datastructure/metric_object.hpp"
#include "../../geometry/algorithms/predicates.hpp"
//
// *** CGAL includes
//
// 2D part
//
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/Triangulation_conformer_2.h>
//
// 3D part
//
#include <CGAL/AABB_intersections.h>  // [JW] has to be first!
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>
#include <CGAL/Polyhedral_mesh_domain_3.h>
#include <CGAL/make_mesh_3.h>
#include <CGAL/refine_mesh_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>
#include <CGAL/Modifier_base.h>

//
// ===================================================================================
//
namespace gml {

template < class HDS,
           typename PointContainer,
           typename CellContainer >
struct PolyhedronLoader :  public CGAL::Modifier_base< HDS > 
{
   typedef HDS Halfedge_data_structure;

   PolyhedronLoader( PointContainer&   point_cont,
                     CellContainer&    cell_cont )
   : _point_cont( point_cont ),
     _cell_cont ( cell_cont  ) {}
  
   void operator()( HDS& target )
   {
      CGAL::Polyhedron_incremental_builder_3< HDS > B( target, 1 );

      B.begin_surface( _point_cont.size(), _cell_cont.size() );
    
      for( typename PointContainer::const_iterator iter = _point_cont.begin();
           iter != _point_cont.end(); ++iter )
      {
         B.add_vertex( *iter );
      }
    
      for( typename CellContainer::const_iterator iter = _cell_cont.begin();
           iter != _cell_cont.end(); ++iter )
      {
         B.begin_facet();

         for( typename CellContainer::value_type::const_iterator vociter = iter->begin();
              vociter != iter->end(); vociter++ )
         {
            B.add_vertex_to_facet( *vociter );
         }
         B.end_facet();
      }     
      B.end_surface();  
   }
  
   PointContainer& _point_cont;
   CellContainer&  _cell_cont;
};

//
// ===================================================================================
//
template < int DIM, typename NumericT >
struct Cgal { };
//
// ===================================================================================
//
template < typename NumericT >
struct Cgal < 3, NumericT >
{
   // -------------------------------------------------------------------------
   typedef NumericT  Numeric;
   typedef long       Index;

   typedef gml::metric_object < Numeric , 3 >      Coord;
   typedef std::vector < Coord >                   ResultPointContainer;

   typedef gml::metric_object < Index , 4 >        Cell;
   typedef std::vector < Cell >                    ResultCellContainer;   
   // -------------------------------------------------------------------------
   // CGAL TYPES
   //
   typedef CGAL::Exact_predicates_inexact_constructions_kernel       K;
   typedef CGAL::Polyhedron_3<K>                                     Polyhedron;
   typedef typename Polyhedron::HalfedgeDS                           HalfedgeDS;   
   typedef CGAL::Polyhedral_mesh_domain_3<Polyhedron, K>             Mesh_domain;
   typedef CGAL::Mesh_triangulation_3<Mesh_domain>::type             Tr;
   typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr>               C3t3;
   typedef CGAL::Mesh_criteria_3<Tr>                                 Mesh_criteria;
   typedef Mesh_criteria::Facet_criteria                             Facet_criteria;
   typedef Mesh_criteria::Cell_criteria                              Cell_criteria;
   typedef K::Point_3                                                Point;
   // -------------------------------------------------------------------------
   typedef std::vector< Point >                    InputPointContainer;
   typedef gml::metric_object< long, 3 >           BoundaryFace;
   typedef std::vector< BoundaryFace >             InputBoundaryContainer;
   // -------------------------------------------------------------------------
   Cgal()
   {
   #ifdef MESH_DEBUG
      std::cout << "GML::MeshEngine::Cgal::initiating .." << std::endl;
   #endif
   }
   // -------------------------------------------------------------------------
   template<typename PointT>
   void add(PointT const& pnt,            // const-reference specialization
            typename boost::enable_if< 
               typename boost::is_floating_point<
                  typename gml::result_of::val< PointT >::type > >::type* dummy = 0) 
   {
      input_points.push_back( Point( pnt[0], pnt[1], pnt[2] ) );
   }
   template<typename PointT>
   void add(PointT & pnt,                 // reference specialization
            typename boost::enable_if< 
               typename boost::is_floating_point<
                  typename gml::result_of::val< PointT >::type > >::type* dummy = 0) 
   {
      input_points.push_back( Point( pnt[0], pnt[1], pnt[2] ) );
   }      
   // -------------------------------------------------------------------------      
   template <typename PointIndicesT>
   void add(PointIndicesT const& indices, // const-reference specialization
            typename boost::enable_if< 
               typename boost::is_integral<
                  typename gml::result_of::val< PointIndicesT >::type > >::type* dummy = 0) 
   {  
      input_boundary.push_back( BoundaryFace( indices[0], indices[1], indices[2] ) );
   }
   template <typename PointIndicesT>
   void add(PointIndicesT & indices,      // reference specialization
            typename boost::enable_if< 
               typename boost::is_integral<
                  typename gml::result_of::val< PointIndicesT >::type > >::type* dummy = 0) 
   {
      gml::metric_object< double, 3 >  a, b, c;

      for( int i=0; i < 3; i++ ) 
      {
         a[i] = input_points[indices[0]][i];
         b[i] = input_points[indices[1]][i];
         c[i] = input_points[indices[2]][i];         
      }

      double orient = gml::algorithms::orient2d_geomdim3( a, b, c );
   
      if( orient > 0 ) // counter-clockwise
         input_boundary.push_back( BoundaryFace( indices[0], indices[1], indices[2] ) );   
      else
      {
         std::cout << "INTERFACE::CGAL::ADD-CELL - ENCOUNTERED WRONG ORIENTATION - NOT IMPLEMENTED!" << std::endl;
         std::cout << "shutting down .. "<< std::endl;
         exit(-1);
      }
      
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
   void start()
   {
   #ifdef MESH_DEBUG
      std::cout << "GML::MeshEngine::Cgal: starting meshing process .." << std::endl;
   #endif   

      typedef gml::PolyhedronLoader< 
         HalfedgeDS, InputPointContainer, InputBoundaryContainer
      >  PolyhedronLoader;
      
      PolyhedronLoader poly_loader( input_points, input_boundary );
   
      polyhedron.delegate( poly_loader );
      /*
      std::cout << "is polyhedron valid: " << polyhedron.is_valid() << std::endl;
      std::cout << "is normalized border valid: " << polyhedron.normalized_border_is_valid() << std::endl;
      */
      polyhedron.normalize_border();
      /*
      std::cout << "is normalized border valid: " << polyhedron.normalized_border_is_valid() << std::endl;      
      std::cout << "is pure triangle: " << polyhedron.is_pure_triangle() << std::endl;
      std::cout << "is closed: " << polyhedron.is_closed() << std::endl;
      std::cout << "is pure bivalent: " << polyhedron.is_pure_bivalent() << std::endl;
      std::cout << "size of vertices: " << polyhedron.size_of_vertices() << std::endl;   
      std::cout << "size of facets: " << polyhedron.size_of_facets() << std::endl;            
      */
      Mesh_domain    domain( polyhedron );
   
      Facet_criteria facet_criteria(25, 6, 4); // angle, size, approximation
      Cell_criteria cell_criteria(4, 2); // radius-edge ratio, size
      Mesh_criteria criteria(facet_criteria, cell_criteria);
  
      C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria);

      const Tr& tr = c3t3.triangulation();

      gsse::resize( tr.number_of_vertices() )( point_cont_result );

      std::map<typename Tr::Vertex_handle, int> V;
      int inum = 0;
      for( typename Tr::Finite_vertices_iterator vit = tr.finite_vertices_begin();
           vit != tr.finite_vertices_end();
           ++vit)
      {

         Point p = vit->point();
         
         gsse::at( inum )( point_cont_result ) = Coord( (vit->point())[0], 
                                                        (vit->point())[1],
                                                        (vit->point())[2] );
         V[vit] = inum++;
      }

      gsse::resize( c3t3.number_of_cells() )( cell_cont_result );

      inum = 0;
      for( typename C3t3::Cell_iterator cit = c3t3.cells_begin(); 
           cit != c3t3.cells_end(); cit++ )
      {
         gsse::at( inum)(cell_cont_result) = Cell( V[ cit->vertex(0) ],
                                                   V[ cit->vertex(1) ],
                                                   V[ cit->vertex(2) ],  
                                                   V[ cit->vertex(3) ] );      
         inum++;
      }
   
   }   
   // -------------------------------------------------------------------------   
   Polyhedron              polyhedron;
   
   InputBoundaryContainer  input_boundary;
   InputPointContainer     input_points;
   
   ResultPointContainer       point_cont_result;      
   ResultCellContainer        cell_cont_result;         
};
//
// ===================================================================================
//
template < typename NumericT >
struct Cgal < 2, NumericT >
{
   // -------------------------------------------------------------------------
   typedef NumericT  Numeric;
   typedef int       Index;

   typedef gml::metric_object < Numeric , 2 >      Coord;
   typedef std::vector < Coord >                   ResultPointContainer;

   typedef gml::metric_object < Index , 3 >        Cell;
   typedef std::vector < Cell >                    ResultCellContainer;   
   // -------------------------------------------------------------------------
   // CGAL TYPES
   //
   typedef CGAL::Exact_predicates_inexact_constructions_kernel       K;
   typedef CGAL::Triangulation_vertex_base_2<K>                      Vb;
   typedef CGAL::Delaunay_mesh_face_base_2<K>                        Fb;
   typedef CGAL::Triangulation_data_structure_2<Vb, Fb>              Tds;
   typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds>        CDT;
   typedef CGAL::Delaunay_mesh_size_criteria_2<CDT>                  Criteria;
   typedef CDT::Point                                                Point;
   typedef CDT::Vertex_handle                                        Vertex_handle;
   typedef CDT::Finite_vertices_iterator                             Finite_vertices_iterator;
   typedef CDT::Finite_faces_iterator                                Finite_faces_iterator;
   // -------------------------------------------------------------------------
   Cgal()
   {
   #ifdef MESH_DEBUG
      std::cout << "GML::MeshEngine::Cgal::initiating .." << std::endl;
   #endif
      ConvexFlag              = false;
      ConstrainedDelaunayFlag = false;
      ConformingDelaunayFlag  = false;
      ConformingGabrielFlag   = false;
      PersonalCriteriaFlag    = false;
      aspect_bound            = 0.125;
      size_bound              = 0.0;
   }
   // -------------------------------------------------------------------------
   void add(gml::property::ConstrainedDelaunay const& ) 
   {  
   #ifdef MESH_DEBUG
      std::cout << "GML::MeshEngine::Cgal::SetProperty: Constrained Delaunay .." << std::endl;
   #endif      
      ConstrainedDelaunayFlag = true;
      ConformingDelaunayFlag  = false;
      ConformingGabrielFlag   = false;   
      ConvexFlag              = false; 
   }  
   void add(gml::property::ConformingDelaunay const& ) 
   {  
   #ifdef MESH_DEBUG
      std::cout << "GML::MeshEngine::Cgal::SetProperty: Conforming Delaunay .." << std::endl;
   #endif   
      ConformingDelaunayFlag  = true;
      ConstrainedDelaunayFlag = false;   
      ConformingGabrielFlag   = false;   
      ConvexFlag              = false;   
   }
   void add(gml::property::ConformingGabriel const& ) 
   {  
   #ifdef MESH_DEBUG
      std::cout << "GML::MeshEngine::Cgal::SetProperty: Conforming Gabriel .." << std::endl;
   #endif      
      ConformingGabrielFlag   = true;   
      ConstrainedDelaunayFlag = false;
      ConformingDelaunayFlag  = false;
      ConvexFlag              = false;   
   }    
   void add(gml::property::Convex const& ) 
   {  
   #ifdef MESH_DEBUG
      std::cout << "GML::MeshEngine::Cgal::SetProperty: Convex .." << std::endl;
   #endif      
      ConvexFlag              = true;
      ConstrainedDelaunayFlag = false;
      ConformingDelaunayFlag  = false;
      ConformingGabrielFlag   = false;      
   }   
   void add(gml::property::size const& size) // maximum volume constraint
   {  
   #ifdef MESH_DEBUG
      std::cout << "GML::MeshEngine::Cgal::SetProperty: Maximum Size .." << std::endl;
   #endif      
      PersonalCriteriaFlag = true;   
      size_bound = size._val;
   }   
   void add(gml::property::radius_edge_ratio const& radius_edge_ratio) 
   {  
   #ifdef MESH_DEBUG
      std::cout << "GML::MeshEngine::Cgal::SetProperty: Minimum Radius-Edge Ratio .." << std::endl;
   #endif      
      PersonalCriteriaFlag = true;
      aspect_bound = radius_edge_ratio._val;
   }    
   void add(gml::property::angle const& angle) 
   {  
      std::cout << "GML::MeshEngine::Cgal::SetProperty: Angle .." << std::endl;
      std::cout << " ## WARNING ## There is no angle property available" << std::endl;
   } 
   // -------------------------------------------------------------------------
   template<typename PointT>
   void add(PointT const& pnt,            // const-reference specialization
            typename boost::enable_if< 
               typename boost::is_floating_point<
                  typename gml::result_of::val< PointT >::type > >::type* dummy = 0) 
   {   
      cgal_vertex_cont.push_back( cdt.insert( Point( pnt[0], pnt[1] ) ) );
   }
   template<typename PointT>
   void add(PointT & pnt,                 // reference specialization
            typename boost::enable_if< 
               typename boost::is_floating_point<
                  typename gml::result_of::val< PointT >::type > >::type* dummy = 0) 
   {
      cgal_vertex_cont.push_back( cdt.insert( Point( pnt[0], pnt[1] ) ) );
   }   
   // -------------------------------------------------------------------------
   template <typename PointIndicesT>
   void add(PointIndicesT const& indices, // const-reference specialization
            typename boost::enable_if< 
               typename boost::is_integral<
                  typename gml::result_of::val< PointIndicesT >::type > >::type* dummy = 0) 
   {
      cdt.insert_constraint( cgal_vertex_cont[indices[0]], cgal_vertex_cont[indices[1]] );
   }
   template <typename PointIndicesT>
   void add(PointIndicesT & indices,      // reference specialization
            typename boost::enable_if< 
               typename boost::is_integral<
                  typename gml::result_of::val< PointIndicesT >::type > >::type* dummy = 0) 
   {
      cdt.insert_constraint( cgal_vertex_cont[indices[0]], cgal_vertex_cont[indices[1]] );
   }
   // -------------------------------------------------------------------------     
   template <typename PointT>
   void add_hole(PointT const& hole_point)
   {
      list_of_seeds.push_back( Point(hole_point[0], hole_point[1]) );
   }
   template <typename PointT>
   void add_hole(PointT& hole_point)
   {
      list_of_seeds.push_back( Point(hole_point[0], hole_point[1]) );
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
   void start()
   {
   #ifdef MESH_DEBUG
      std::cout << "GML::MeshEngine::Cgal: starting meshing process .." << std::endl;
   #endif   
      //
      // initial mesh process --> creates a constrained delaunay mesh
      //
      //    brief: we may use the default criteria provided by CGAL, 
      //           or we may use new ones ..
      //
      if(!ConvexFlag)
      {
         if( PersonalCriteriaFlag )
         {
            std::cout << "  constrained delaunay with own criterias .. " << std::endl;                                    
            CGAL::refine_Delaunay_mesh_2(
               cdt, 
               list_of_seeds.begin(), list_of_seeds.end(),
               Criteria(aspect_bound, size_bound) 
            );
         }
         else
         {
            std::cout << "  constrained delaunay with default criterias .. " << std::endl;                           
            CGAL::refine_Delaunay_mesh_2(
               cdt, 
               list_of_seeds.begin(), list_of_seeds.end(),
               Criteria() 
            );
         }
         //
         // make the constrained delaunay mesh, conforming delaunay
         //
         if( ConformingDelaunayFlag )
         {
         #ifdef MESH_DEBUG
            std::cout << "  conforming delaunay .." << std::endl;
         #endif            
            CGAL::make_conforming_Delaunay_2(cdt);
         }
         //
         // or even conforming gabriel ..
         //
         else
         if( ConformingGabrielFlag )
         {
            CGAL::make_conforming_Gabriel_2(cdt);
         #ifdef MESH_DEBUG
            std::cout << "  conforming gabriel .." << std::endl;
         #endif                    
         }
      }      
      // ---------------------------------------------------
      //
      // extracting geometry data
      //
      long number_of_vertices(0);
      
      std::pair< Finite_vertices_iterator, Finite_vertices_iterator >   
         point_range(cdt.finite_vertices_begin(), cdt.finite_vertices_end());

      // [JW] TODO is there a size() ? 
      gml::for_each( point_range, gml::ref( number_of_vertices )++ );
      
      gsse::resize( number_of_vertices )( point_cont_result );
      
      long vi = 0;
      for(Finite_vertices_iterator vit = cdt.finite_vertices_begin(); 
          vit != cdt.finite_vertices_end(); ++vit)
      {
         gsse::at( vi )( point_cont_result ) = Coord( (vit->point())[0], (vit->point())[1] );
         //
         // register the geometrical point by assigning an index ..
         //
         geometry_index_map[vit->point()] = vi;
         vi++;
      }
      //
      // extracting topology data
      //
      for(Finite_faces_iterator fit = cdt.finite_faces_begin(); 
          fit != cdt.finite_faces_end(); ++fit)
      {
         if(fit->is_in_domain())
         {
            //
            // retrieve the geometrical points of the triangle, retrieve the corresponding index from
            // the map and add it to the topology view ..
            //
            cell_cont_result.push_back( Cell(geometry_index_map[ ((cdt.triangle(fit))[0]) ],
                                             geometry_index_map[ ((cdt.triangle(fit))[1]) ],
                                             geometry_index_map[ ((cdt.triangle(fit))[2]) ]  ) );
         }
      }         
   }
   // -------------------------------------------------------------------------      
   CDT                              cdt;
   std::vector<Vertex_handle>       cgal_vertex_cont;         

   std::map<Point, long>            geometry_index_map;

   bool           ConvexFlag;
   bool           ConstrainedDelaunayFlag;   
   bool           ConformingDelaunayFlag;
   bool           ConformingGabrielFlag;   
   bool           PersonalCriteriaFlag;
   
   double         aspect_bound;
   double         size_bound;
   std::list<Point>           list_of_seeds;
   
   ResultPointContainer       point_cont_result;      
   ResultCellContainer        cell_cont_result;      
   // -------------------------------------------------------------------------   
};

} // end namespace: gml
#endif

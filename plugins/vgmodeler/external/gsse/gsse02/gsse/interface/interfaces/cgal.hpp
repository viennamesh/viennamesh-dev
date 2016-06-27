/* ============================================================================
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at       
   Copyright (c) 2007-2010 Franz Stimpfl                          franz@gsse.at   

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GMI_INTERFACES_CGAL   
#define GMI_INTERFACES_CGAL
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
// *** GSSE includes
//
#include "gsse/geometry/metric_object.hpp"
#include "gsse/util/common.hpp"
#include "gsse/util/resize.hpp"
#include "gsse/util/access.hpp"
//
// *** GMI includes
//
#include "../utils/for_each.hpp"
//
// *** CGAL includes
//
// 2D part
//
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
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
#include <CGAL/convex_hull_2.h>
#include <CGAL/Mesh_3/Robust_intersection_traits_3.h>
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
namespace gsse {

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
// //
// // ===================================================================================
// //
template < typename NumericT >
struct Cgal < 3, NumericT >
{
   // -------------------------------------------------------------------------
   typedef NumericT  Numeric;
   typedef long       Index;

   typedef gsse::metric_object < Numeric , 3 >      Coord;
   typedef std::vector < Coord >                   ResultPointContainer;

   typedef gsse::metric_object < Index , 4 >        Cell;
   typedef std::vector < Cell >                    ResultCellContainer;   
   // -------------------------------------------------------------------------
   // CGAL TYPES
   //
   typedef CGAL::Exact_predicates_inexact_constructions_kernel       K;
   typedef CGAL::Mesh_3::Robust_intersection_traits_3<K>             Geom_traits;
   typedef CGAL::Polyhedron_3<Geom_traits>                           Polyhedron;
   typedef typename Polyhedron::HalfedgeDS                           HalfedgeDS;   
   typedef CGAL::Polyhedral_mesh_domain_3<Polyhedron, Geom_traits>   Mesh_domain;
   typedef CGAL::Mesh_triangulation_3<Mesh_domain>::type             Tr;
   typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr>               C3t3;
   typedef CGAL::Mesh_criteria_3<Tr>                                 Mesh_criteria;
   typedef Mesh_criteria::Facet_criteria                             Facet_criteria;
   typedef Mesh_criteria::Cell_criteria                              Cell_criteria;
   typedef K::Point_3                                                Point;
   // -------------------------------------------------------------------------
   typedef std::vector< Point >                    InputPointContainer;
   typedef gsse::metric_object< long, 3 >           BoundaryFace;
   typedef std::vector< BoundaryFace >             InputBoundaryContainer;
   // -------------------------------------------------------------------------
   Cgal()
   {
   #ifdef MESH_DEBUG
      std::cout << "gmi::MeshEngine::Cgal::initiating .." << std::endl;
   #endif
   }
   // -------------------------------------------------------------------------
   void add(gmi::property::ConformingDelaunay const& ) 
   {  
   #ifdef MESH_DEBUG
      std::cout << "gmi::MeshEngine::Cgal::SetProperty: Conforming Delaunay .." << std::endl;
   #endif   
//       ConformingDelaunayFlag  = true;
//       ConstrainedDelaunayFlag = false;   
//       ConformingGabrielFlag   = false;   
//       ConvexFlag              = false;    
   }
   // -------------------------------------------------------------------------
   template<typename PointT>
   void add(PointT const& pnt,            // const-reference specialization
            typename boost::enable_if< 
               typename boost::is_floating_point<
                  typename gsse::result_of::val< PointT >::type > >::type* dummy = 0) 
   {
      input_points.push_back( Point( pnt[0], pnt[1], pnt[2] ) );
   }
   template<typename PointT>
   void add(PointT & pnt,                 // reference specialization
            typename boost::enable_if< 
               typename boost::is_floating_point<
                  typename gsse::result_of::val< PointT >::type > >::type* dummy = 0) 
   {
      input_points.push_back( Point( pnt[0], pnt[1], pnt[2] ) );
   }      
   // -------------------------------------------------------------------------      
   template <typename PointIndicesT>
   void add(PointIndicesT const& indices, // const-reference specialization
            typename boost::enable_if< 
               typename boost::is_integral<
                  typename gsse::result_of::val< PointIndicesT >::type > >::type* dummy = 0) 
   {  
      input_boundary.push_back( BoundaryFace( indices[0], indices[1], indices[2] ) );
   }
   template <typename PointIndicesT>
   void add(PointIndicesT & indices,      // reference specialization
            typename boost::enable_if< 
               typename boost::is_integral<
                  typename gsse::result_of::val< PointIndicesT >::type > >::type* dummy = 0) 
   {
      gsse::metric_object< double, 3 >  a, b, c;

      for( int i=0; i < 3; i++ ) 
      {
         a[i] = input_points[indices[0]][i];
         b[i] = input_points[indices[1]][i];
         c[i] = input_points[indices[2]][i];         
      }

      // [TODO][FS] change this
      double orient = 1; // gmi::algorithms::orient2d_geomdim3( a, b, c );

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
      std::cout << "gmi::MeshEngine::Cgal: starting meshing process .." << std::endl;
   #endif   

      typedef gsse::PolyhedronLoader< 
         HalfedgeDS, InputPointContainer, InputBoundaryContainer
      >  PolyhedronLoader;
   
      PolyhedronLoader poly_loader( input_points, input_boundary );

//       std::ifstream input("data/elephant.off");
//       input >> polyhedron;
   
      polyhedron.delegate( poly_loader );
   
      std::cout << "is polyhedron valid: " << polyhedron.is_valid() << std::endl;
      std::cout << "is normalized border valid: " << polyhedron.normalized_border_is_valid() << std::endl;
   
      polyhedron.normalize_border();
   
      std::cout << "is normalized border valid: " << polyhedron.normalized_border_is_valid() << std::endl;      
      std::cout << "is pure triangle: " << polyhedron.is_pure_triangle() << std::endl;
      std::cout << "is closed: " << polyhedron.is_closed() << std::endl;
      std::cout << "is pure bivalent: " << polyhedron.is_pure_bivalent() << std::endl;
      std::cout << "size of vertices: " << polyhedron.size_of_vertices() << std::endl;   
      std::cout << "size of facets: " << polyhedron.size_of_facets() << std::endl;            

      Mesh_domain    domain( polyhedron );

//       Facet_criteria facet_criteria(25, 0.15, 0.008); // angle, size, approximation
//       Cell_criteria cell_criteria(4, 0.2); // radius-edge ratio, size
//       Mesh_criteria criteria(facet_criteria, cell_criteria);

//       C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria);

//       Mesh_criteria criteria(CGAL::parameters::facet_angle=25, CGAL::parameters::facet_size=0.15, 
//                              CGAL::parameters::facet_distance=0.008, CGAL::parameters::cell_radius_edge=3);      

      Mesh_criteria criteria(CGAL::parameters::facet_angle=25, CGAL::parameters::facet_size=1.15, 
                             CGAL::parameters::facet_distance=0.008, CGAL::parameters::cell_radius_edge=3);      

      // Mesh generation
      C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria, CGAL::parameters::no_perturb(), CGAL::parameters::no_exude());

      const Tr& tr = c3t3.triangulation();

      // [TODO] include refinement
//       Mesh_criteria new_criteria(cell_radius_edge=3, cell_size=0.03);
//       CGAL::refine_mesh_3(c3t3, domain, new_criteria);

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

   ResultPointContainer    point_cont_result;      
   ResultCellContainer     cell_cont_result;         
};

// //
// // ===================================================================================
// //
template < typename NumericT >
struct Cgal < 2, NumericT >
{
   // -------------------------------------------------------------------------
   typedef NumericT  Numeric;
   typedef int       Index;

   typedef gsse::metric_object < Numeric , 2 >      Coord;
   typedef std::vector < Coord >                   ResultPointContainer;

   typedef gsse::metric_object < Index , 3 >        Cell;
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
      std::cout << "gmi::MeshEngine::Cgal::initiating .." << std::endl;
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
   void add(gmi::property::ConstrainedDelaunay const& ) 
   {  
   #ifdef MESH_DEBUG
      std::cout << "gmi::MeshEngine::Cgal::SetProperty: Constrained Delaunay .." << std::endl;
   #endif      
      ConstrainedDelaunayFlag = true;
      ConformingDelaunayFlag  = false;
      ConformingGabrielFlag   = false;   
      ConvexFlag              = false; 
   }  
   void add(gmi::property::ConformingDelaunay const& ) 
   {  
   #ifdef MESH_DEBUG
      std::cout << "gmi::MeshEngine::Cgal::SetProperty: Conforming Delaunay .." << std::endl;
   #endif   
      ConformingDelaunayFlag  = true;
      ConstrainedDelaunayFlag = false;   
      ConformingGabrielFlag   = false;   
      ConvexFlag              = false;   
   }
   void add(gmi::property::ConformingGabriel const& ) 
   {  
   #ifdef MESH_DEBUG
      std::cout << "gmi::MeshEngine::Cgal::SetProperty: Conforming Gabriel .." << std::endl;
   #endif      
      ConformingGabrielFlag   = true;   
      ConstrainedDelaunayFlag = false;
      ConformingDelaunayFlag  = false;
      ConvexFlag              = false;   
   }    
   void add(gmi::property::Convex const& ) 
   {  
   #ifdef MESH_DEBUG
      std::cout << "gmi::MeshEngine::Cgal::SetProperty: Convex .." << std::endl;
   #endif      
      ConvexFlag              = true;
      ConstrainedDelaunayFlag = false;
      ConformingDelaunayFlag  = false;
      ConformingGabrielFlag   = false;      
   }   
   void add(gmi::property::size const& size) // maximum volume constraint
   {  
   #ifdef MESH_DEBUG
      std::cout << "gmi::MeshEngine::Cgal::SetProperty: Maximum Size .." << std::endl;
   #endif      
      PersonalCriteriaFlag = true;   
      size_bound = size._val;
   }   
   void add(gmi::property::radius_edge_ratio const& radius_edge_ratio) 
   {  
   #ifdef MESH_DEBUG
      std::cout << "gmi::MeshEngine::Cgal::SetProperty: Minimum Radius-Edge Ratio .." << std::endl;
   #endif      
      PersonalCriteriaFlag = true;
      aspect_bound = radius_edge_ratio._val;
   }    
   void add(gmi::property::angle const& angle) 
   {  
      std::cout << "gmi::MeshEngine::Cgal::SetProperty: Angle .." << std::endl;
      std::cout << " ## WARNING ## There is no angle property available" << std::endl;
   } 
   // -------------------------------------------------------------------------
   template<typename PointT>
   void add(PointT const& pnt,            // const-reference specialization
            typename boost::enable_if< 
               typename boost::is_floating_point<
                  typename gsse::result_of::val< PointT >::type > >::type* dummy = 0) 
   {   
      cgal_vertex_cont.push_back( cdt.insert( Point( pnt[0], pnt[1] ) ) );
   }
   template<typename PointT>
   void add(PointT & pnt,                 // reference specialization
            typename boost::enable_if< 
               typename boost::is_floating_point<
                  typename gsse::result_of::val< PointT >::type > >::type* dummy = 0) 
   {
      cgal_vertex_cont.push_back( cdt.insert( Point( pnt[0], pnt[1] ) ) );
   }   
   // -------------------------------------------------------------------------
   template <typename PointIndicesT>
   void add(PointIndicesT const& indices, // const-reference specialization
            typename boost::enable_if< 
               typename boost::is_integral<
                  typename gsse::result_of::val< PointIndicesT >::type > >::type* dummy = 0) 
   {
      cdt.insert_constraint( cgal_vertex_cont[indices[0]], cgal_vertex_cont[indices[1]] );
   }
   template <typename PointIndicesT>
   void add(PointIndicesT & indices,      // reference specialization
            typename boost::enable_if< 
               typename boost::is_integral<
                  typename gsse::result_of::val< PointIndicesT >::type > >::type* dummy = 0) 
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
      std::cout << "gmi::MeshEngine::Cgal: starting meshing process .." << std::endl;
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

      gmi::for_each( point_range, boost::phoenix::ref( number_of_vertices )++ );
   
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

template<typename PointContainerT>
struct convex_hull
{
   typedef typename PointContainerT::value_type PointT;

   typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
   typedef K::Point_2 Point_2;
   typedef std::vector<Point_2> Points;

   void operator()(PointContainerT& in_points)
   {
      Points points, result;
      
      for(int i = 0; i < in_points.size(); ++i)
         points.push_back(Point_2(in_points[i][0], in_points[i][1]));

      CGAL::convex_hull_2( points.begin(), points.end(), std::back_inserter(result) );
      std::cout << result.size() << " points on the convex hull" << std::endl;
   }
};

namespace mesh 
{
namespace result_of
{
   struct cgal2d_wrapper
   {
      typedef gsse::mesh::strategy::constrained_delaunay strategy_type;
            
      static const int topological_input_dimension_tag  = 1;
      static const int geometrical_input_dimension_tag  = 2;
            
      static const int topological_output_dimension_tag = 2;
      static const int geometrical_output_dimension_tag = 2;
            
      template<typename InputSpaceT, typename OutputSpaceT>
      void operator()(InputSpaceT& input_space, OutputSpaceT& output_space)
      {
         std::cout << "\t## generate constrained delaunay 2D mesh" << std::endl;
//               generate_incremental_delaunay_mesh<InputSpaceT, OutputSpaceT>(input_space, output_space);
               
         // [TODO] include multi precision libraries 
         typedef gsse::Cgal<2, double>  Mesher;

//          typedef gsse::property_domain<OutputSpaceT>  GSSE_Domain2;   
//          typedef typename gsse::result_of::property_CoordPosT<GSSE_Domain2>::type              CoordPosT2;
//          typedef typename gsse::result_of::property_CellT<GSSE_Domain2>::type                  CellT2;   
//          typedef typename gsse::result_of::property_SpaceTopologySegmentsT<GSSE_Domain2>::type SpaceTopologySegmentsT2;
//          typedef typename gsse::result_of::property_SpaceQuantitySegmentsT<GSSE_Domain2>::type SpaceQuantitySegmentsT2;
//          typedef typename gsse::result_of::property_FBPosT<GSSE_Domain2>::type                 FBPosT2;

         typedef typename gsse::result_of::at_dim<OutputSpaceT, gsse::access_specifier::AC>::type    SpaceTopologySegmentsT;
         typedef typename gsse::result_of::at_dim<OutputSpaceT, gsse::access_specifier::AQ>::type    SpaceQuantitySegmentsT;
         typedef typename gsse::result_of::at_dim<OutputSpaceT, gsse::access_specifier::AP>::type    FBPosT;
         typedef typename gsse::result_of::at_dim_isd  <FBPosT>::type                                CoordPosT;

         typedef typename gsse::result_of::at_dim_index<SpaceTopologySegmentsT>::type                CellComplex;  
         typedef typename gsse::result_of::at_dim<CellComplex, gsse::access_specifier::AT_cl>::type  ContainerVXonCL;
         typedef typename gsse::result_of::val<ContainerVXonCL>::type                                CellT;
      
         typedef typename gsse::result_of::at_dim_index<SpaceQuantitySegmentsT>::type  QuanComplex;
         typedef typename gsse::result_of::at_dim<QuanComplex, AT_vx>::type    QuanDynamicForm;  // [RH][TODO]
         typedef typename gsse::result_of::at_isd<QuanDynamicForm, 1>::type        StorageDynamicForm;
            
         SpaceTopologySegmentsT& segments_topology_output = gsse::at_dim<AC>(output_space);
//      SpaceQuantitySegmentsT& segments_quantity_output = gsse::at_dim<AQ>(output_space);
         FBPosT&                 geometry_output          = gsse::at_dim<AP>(output_space);                  
         Mesher mesher;
           
         // [INFO] transfer points to mesher
         for(size_t gpi = 0; gpi < gsse::size(gsse::at_dim<AP>(input_space)); ++gpi)
            gmi::add( gsse::at(gpi)(gsse::at_dim<AP>(input_space)) ) (mesher);
               
         // [INFO] transfer cells to mesher
         for(size_t si = 0; si < gsse::size(gsse::at_dim<AC>(input_space)); ++si)
            for (long ci = 0 ; ci < gsse::size_cl(gsse::at(si)(gsse::at_dim<AC>(input_space))); ++ci)
               gmi::add( gsse::at_cl(ci)(gsse::at(si)(gsse::at_dim<AC>(input_space))) ) (mesher);   
   
         // [TODO] set strategy at creation time of object
//         gmi::add( gmi::property::conforming_delaunay )  (mesher);
         gmi::add( gmi::property::constrained_delaunay )  (mesher);

         
         // [TODO] find a way to specify holes
         //   gmi::add_hole(Point(0.5,0.5))(mesher);
         
         gmi::start( mesher );      

         // [INFO] write mesh back to space
         for(size_t i = 0; i < gsse::size( gmi::geometry(mesher) ); i++ )
         {
            CoordPosT temp_point;
                  
            for(size_t pi = 0; pi < gsse::size( gmi::geometry(mesher)[i] ); ++pi)
            {
               gsse::at(pi)(temp_point) = gmi::geometry(mesher)[i][pi];
            }            
            gsse::at(i)(geometry_output) = temp_point;         
         }
      
         for(size_t ci = 0; ci < gsse::size( gmi::topology(mesher) ); ci++ )
         {
            CellT temp_cell;
         
            for(size_t cii = 0; cii < gsse::size( gmi::topology(mesher)[ci] ); ++cii)
            {
               gsse::at(cii)(temp_cell) = gmi::topology(mesher)[ci][cii];
            }         
            gsse::pushback(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(0)(segments_topology_output)))) = temp_cell;
         }
      }         
   };


   struct cgal3d_wrapper
   {
      typedef gsse::mesh::strategy::constrained_delaunay strategy_type;
            
      static const int topological_input_dimension_tag  = 2;
      static const int geometrical_input_dimension_tag  = 3;
            
      static const int topological_output_dimension_tag = 3;
      static const int geometrical_output_dimension_tag = 3;
            
      template<typename InputSpaceT, typename OutputSpaceT>
      void operator()(InputSpaceT& input_space, OutputSpaceT& output_space)
      {
         std::cout << "\t## generate constrained delaunay 3D mesh" << std::endl;
//               generate_incremental_delaunay_mesh<InputSpaceT, OutputSpaceT>(input_space, output_space);
               
         // [TODO] include multi precision libraries 
         typedef gsse::Cgal<3, double>  Mesher;

//          typedef gsse::property_domain<OutputSpaceT>  GSSE_Domain2;   
//          typedef typename gsse::result_of::property_CoordPosT<GSSE_Domain2>::type              CoordPosT2;
//          typedef typename gsse::result_of::property_CellT<GSSE_Domain2>::type                  CellT2;   
//          typedef typename gsse::result_of::property_SpaceTopologySegmentsT<GSSE_Domain2>::type SpaceTopologySegmentsT2;
//          typedef typename gsse::result_of::property_SpaceQuantitySegmentsT<GSSE_Domain2>::type SpaceQuantitySegmentsT2;
//          typedef typename gsse::result_of::property_FBPosT<GSSE_Domain2>::type                 FBPosT2;

         typedef typename gsse::result_of::at_dim<OutputSpaceT, gsse::access_specifier::AC>::type    SpaceTopologySegmentsT;
         typedef typename gsse::result_of::at_dim<OutputSpaceT, gsse::access_specifier::AQ>::type    SpaceQuantitySegmentsT;
         typedef typename gsse::result_of::at_dim<OutputSpaceT, gsse::access_specifier::AP>::type    FBPosT;
         typedef typename gsse::result_of::at_dim_isd  <FBPosT>::type                                CoordPosT;

         typedef typename gsse::result_of::at_dim_index<SpaceTopologySegmentsT>::type                CellComplex;  
         typedef typename gsse::result_of::at_dim<CellComplex, gsse::access_specifier::AT_cl>::type  ContainerVXonCL;
         typedef typename gsse::result_of::val<ContainerVXonCL>::type                                CellT;
      
//          typedef typename gsse::result_of::at_dim_index<SpaceQuantitySegmentsT2>::type  QuanComplex;
//          typedef typename gsse::result_of::at_dim<QuanComplex, AT_vx>::type    QuanDynamicForm;  // [RH][TODO]
//          typedef typename gsse::result_of::at_isd<QuanDynamicForm, 1>::type        StorageDynamicForm;
            
         SpaceTopologySegmentsT& segments_topology_output = gsse::at_dim<AC>(output_space);
//      SpaceQuantitySegmentsT& segments_quantity_output = gsse::at_dim<AQ>(output_space);
         FBPosT&                 geometry_output          = gsse::at_dim<AP>(output_space);                  
         Mesher mesher;
           
         // [INFO] transfer points to mesher
         for(size_t gpi = 0; gpi < gsse::size(gsse::at_dim<AP>(input_space)); ++gpi)
            gmi::add( gsse::at(gpi)(gsse::at_dim<AP>(input_space)) ) (mesher);
               
         // [INFO] transfer cells to mesher
         for(size_t si = 0; si < gsse::size(gsse::at_dim<AC>(input_space)); ++si)
            for (long ci = 0 ; ci < gsse::size_cl(gsse::at(si)(gsse::at_dim<AC>(input_space))); ++ci)
               gmi::add( gsse::at_cl(ci)(gsse::at(si)(gsse::at_dim<AC>(input_space))) ) (mesher);   
   
         // [TODO] set strategy at creation time of object
         gmi::add( gmi::property::conforming_delaunay )  (mesher);
//         gmi::add( gmi::property::constrained_delaunay )  (mesher);

         
         // [TODO] find a way to specify holes
         //   gmi::add_hole(Point(0.5,0.5))(mesher);
         
         gmi::start( mesher );      

         // [INFO] write mesh back to space
         for(size_t i = 0; i < gsse::size( gmi::geometry(mesher) ); i++ )
         {
            CoordPosT temp_point;
                  
            for(size_t pi = 0; pi < gsse::size( gmi::geometry(mesher)[i] ); ++pi)
            {
               gsse::at(pi)(temp_point) = gmi::geometry(mesher)[i][pi];
            }            
            gsse::at(i)(geometry_output) = temp_point;         
         }
      
         for(size_t ci = 0; ci < gsse::size( gmi::topology(mesher) ); ci++ )
         {
            CellT temp_cell;
         
            for(size_t cii = 0; cii < gsse::size( gmi::topology(mesher)[ci] ); ++cii)
            {
               gsse::at(cii)(temp_cell) = gmi::topology(mesher)[ci][cii];
            }         
            gsse::pushback(gsse::at_dim<AT_cl>(gsse::at_fiber(gsse::at(0)(segments_topology_output)))) = temp_cell;
         }
      }         
   };


} // end namespace: result_of
   
} // end namespace: mesh

} // end namespace: gsse
#endif

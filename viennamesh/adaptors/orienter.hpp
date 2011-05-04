/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_ADAPTORS_ORIENTER_HPP
#define VIENNAMESH_ADAPTORS_ORIENTER_HPP


// *** vienna includes
#include "viennamesh/adaptors/base.hpp"
#include "viennagrid/domain.hpp"

// *** boost includes
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

// *** cgal includes
#include "CGAL/Exact_predicates_exact_constructions_kernel.h"
#include "CGAL/Ray_3.h"
#include "CGAL/intersection_3.h"

//#define MESH_ADAPTOR_DEBUG

/*

basic idea:
get the normal vector of a seed/start/reference cell and determine how often 
the line of this normal vector intersects with faces of the mesh.
if the number of intersections is an even number, the normal vector points outwards.
if it's an odd number, the normal vector points inwards.

*/

namespace viennamesh {

template<typename VectorT>
VectorT ex(VectorT const& v1, VectorT const& v2)
{
   return VectorT(v1[1]*v2[2]-v1[2]*v2[1], 
                  v1[2]*v2[0]-v1[0]*v2[2],
                  v1[0]*v2[1]-v1[1]*v2[0]);
}

template<typename VectorT>
VectorT barycenter(VectorT const& p1, VectorT const& p2, VectorT const& p3)
{
   return VectorT((p1[0] + p2[0] + p3[0])/3.,
                  (p1[1] + p2[1] + p3[1])/3.,
                  (p1[2] + p2[2] + p3[2])/3.);      
}

template <>
struct mesh_adaptor <viennamesh::tag::orienter>
{
   typedef double          numeric_type;
   typedef std::size_t     index_type;

   typedef viennagrid::domain<viennagrid::config::triangular_3d>     domain_type;
   typedef boost::shared_ptr< domain_type >                          input_type;
   typedef input_type                                                result_type;

   typedef domain_type::config_type                     DomainConfiguration;

   typedef DomainConfiguration::numeric_type            CoordType;
   typedef DomainConfiguration::dimension_tag           DimensionTag;
   typedef DomainConfiguration::cell_tag                CellTag;

   typedef domain_type::segment_type                                                                  SegmentType;
   typedef viennagrid::result_of::point_type<DomainConfiguration>::type                               PointType;
   typedef viennagrid::result_of::ncell_type<DomainConfiguration, 0>::type                            VertexType;
   typedef viennagrid::result_of::ncell_type<DomainConfiguration, CellTag::topology_level>::type      CellType;
   typedef viennagrid::result_of::ncell_container<SegmentType, CellTag::topology_level>::type   CellContainer;      
   typedef viennagrid::result_of::iterator<CellContainer>::type                                       CellIterator;      
   typedef viennagrid::result_of::ncell_container<CellType, 0>::type                            VertexOnCellContainer;
   typedef viennagrid::result_of::iterator<VertexOnCellContainer>::type                               VertexOnCellIterator;      
   
   static const int DIMG = DomainConfiguration::dimension_tag::value;
   static const int DIMT = DomainConfiguration::cell_tag::topology_level;   
   static const int CELLSIZE = DIMT+1;      
   
   typedef CGAL::Exact_predicates_inexact_constructions_kernel    K;
   typedef CGAL::Ray_3<K>                                         cgal_ray_type;
   typedef CGAL::Point_3<K>                                       cgal_point_type;
   typedef CGAL::Direction_3<K>                                   cgal_direction_type;
   typedef CGAL::Triangle_3<K>                                    cgal_triangle_type;

   // -------------------------------------------------------------------------------------
   mesh_adaptor() : id("orienter") 
   {
   #ifdef MESH_ADAPTOR_DEBUG
      std::cout << "## MeshAdaptor::"+id+" - initiating .." << std::endl;
   #endif         
   }
   // -------------------------------------------------------------------------------------

   // -------------------------------------------------------------------------------------
   result_type operator()(input_type domain)
   {
   #ifdef MESH_ADAPTOR_DEBUG
      std::cout << "## MeshAdaptor::"+id+" - starting up .." << std::endl;
   #endif            
   
      // traverse the segments ..
      //
      for (std::size_t si = 0; si < domain->segment_size(); ++si)
      {
   #ifdef MESH_ADAPTOR_DEBUG
      std::cout << "## MeshAdaptor::"+id+" - processing segment: " << si << std::endl;
   #endif       
      
         // get the first cell of the segment
         //
         SegmentType & seg = domain->segment(si);
         CellContainer cells = viennagrid::ncells<CellTag::topology_level>(seg);
         
         CellIterator first_cell = cells.begin();
         CellType & seed_cell = *first_cell;
         
         // get the cell points
         //
         boost::array<PointType,CELLSIZE>     cell_points;
         std::size_t vi = 0;       
         VertexOnCellContainer vertices_for_cell = viennagrid::ncells<0>(seed_cell);
         for (VertexOnCellIterator vocit = vertices_for_cell.begin();
             vocit != vertices_for_cell.end();
             ++vocit)
         {
            cell_points[vi++] = vocit->getPoint();
         }
         
         // compute the cells barycenter
         //         
         PointType bc = viennamesh::barycenter(cell_points[0], cell_points[1], cell_points[2]);
         
         // compute the cell normal
         //
         PointType vec1 = cell_points[1]-cell_points[0];
         PointType vec2 = cell_points[2]-cell_points[0];         
         
         PointType normal = viennamesh::ex(vec1,vec2);
         
   #ifdef MESH_ADAPTOR_DEBUG
      std::cout << "  computed normal of cell: " << normal << std::endl;
   #endif                     
         
         // setup the ray
         // note: a ray starts from a point and points in a specific direction
         //       a ray is infinite long ..
         // further note: we use the barycenter as the ray origin, 
         //       this way we ensure that the neighbour triangles are not 
         //       automatically tested positive by the "intersection" algorithm 
         cgal_ray_type  ray(
            cgal_point_type(bc[0], bc[1], bc[2]),
            cgal_direction_type(normal[0], normal[1], normal[2])
         );
 
         std::size_t intersections = 0; 
         
         // traverse the cells ..
         // note: the first cell is the reference cell, we don't have to 
         // check for intersection with this one, so we start at the second one
         //
         first_cell++;
         for (CellIterator cit = first_cell; cit != cells.end(); ++cit)
         {
            CellType & cell = *cit;
            
            // get the cell points
            // note: as this is only done for one cell, we can use a vector here ..
            //
            vi = 0;           
            VertexOnCellContainer vertices_for_cell = viennagrid::ncells<0>(cell);
            for (VertexOnCellIterator vocit = vertices_for_cell.begin();
                vocit != vertices_for_cell.end();
                ++vocit)
            {
               cell_points[vi++] = vocit->getPoint();
            }            
            cgal_triangle_type triangle(
               cgal_point_type(cell_points[0][0], cell_points[0][1], cell_points[0][2]),
               cgal_point_type(cell_points[1][0], cell_points[1][1], cell_points[1][2]),
               cgal_point_type(cell_points[2][0], cell_points[2][1], cell_points[2][2])
            );

            // utilize the cgal intersection algorithm to test if the ray intersects 
            // the triangle ..
            //
            if(CGAL::do_intersect(triangle, ray)) intersections++;
            
         }
      #ifdef MESH_ADAPTOR_DEBUG
         std::cout << "  detected intersections: " << intersections << std::endl;
      #endif                     
      
         // if there is an odd number of intersections, we have to change 
         // the orientation of the reference cell, bevore we proceed 
         // spreading the orientation to all the other cells of this segment
         //
         if( intersections%2 != 0 )
         {
            VertexType *vertices[viennagrid::subcell_traits<CellTag, 0>::num_elements];  

            boost::array<index_type, CELLSIZE>    temp_cell;
            vi = 0;
            VertexOnCellContainer vertices_for_cell = viennagrid::ncells<0>(seed_cell);
            for (VertexOnCellIterator vocit = vertices_for_cell.begin();
                vocit != vertices_for_cell.end();
                ++vocit)
            {
               temp_cell[vi++] = vocit->getID();
            }            

            // return the orientation by using reversed indices of the original cell
            vertices[0] = &(domain->vertex(temp_cell[2]));               
            vertices[1] = &(domain->vertex(temp_cell[1]));
            vertices[2] = &(domain->vertex(temp_cell[0]));

            seed_cell.setVertices(vertices);
            seed_cell.fill(*domain);
         }
         
         // now that the seed cell is oriented correctly, 
         // spread the word to all the other cells of the segment
         //
      }
   
      return domain;
   }
   // -------------------------------------------------------------------------------------
   
   std::string id;
};

} // end namespace viennamesh

#endif










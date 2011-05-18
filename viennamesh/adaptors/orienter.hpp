/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at
               Franz Stimpfl


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_ADAPTORS_ORIENTER_HPP
#define VIENNAMESH_ADAPTORS_ORIENTER_HPP


// *** vienna includes
#include "viennamesh/adaptors/base.hpp"
#include "viennamesh/data.hpp"
#include "viennagrid/domain.hpp"
#include "viennagrid/algorithm/cross_prod.hpp"
#include "viennadata/api.hpp"

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
   typedef viennagrid::result_of::ncell_type<DomainConfiguration, 1>::type                            EdgeType;   
   typedef viennagrid::result_of::ncell_type<DomainConfiguration, CellTag::topology_level>::type      CellType;
   typedef viennagrid::result_of::ncell_container<SegmentType, CellTag::topology_level>::type         CellContainer;      
   typedef viennagrid::result_of::iterator<CellContainer>::type                                       CellIterator;      
   typedef viennagrid::result_of::ncell_container<CellType, 0>::type                                  VertexOnCellContainer;
   typedef viennagrid::result_of::iterator<VertexOnCellContainer>::type                               VertexOnCellIterator;      
   typedef viennagrid::result_of::ncell_container<CellType, 1>::type                                  EdgeOnCellContainer;
   typedef viennagrid::result_of::iterator<EdgeOnCellContainer>::type                                 EdgeOnCellIterator;      
   typedef viennagrid::result_of::ncell_container<EdgeType, CellTag::topology_level>::type            CellOnEdgeContainer;
   typedef viennagrid::result_of::iterator<CellOnEdgeContainer>::type                                 CellOnEdgeIterator;   
   
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
   
      std::size_t global_consistant = 0;
      std::size_t global_leftovers = 0;
      std::size_t global_errors = 0;   
      std::size_t norm_vector_corrections = 0;   
      std::size_t global_corrected_cells = 0;   
   
      // traverse the segments ..
      //
      for (std::size_t si = 0; si < domain->segment_size(); ++si)
      {
   #ifdef MESH_ADAPTOR_DEBUG
      std::cout << "## MeshAdaptor::"+id+" - processing segment: " << si << std::endl;
   #endif       
         SegmentType & seg = domain->segment(si);
         CellContainer cells = viennagrid::ncells<CellTag::topology_level>(seg);

      #ifdef MESH_ADAPTOR_DEBUG         
         std::cout << "  looking for seed cell: " << std::endl;         
      #endif
         CellType seed_cell;
         for (CellIterator cit = cells.begin(); cit != cells.end(); ++cit)
         {
            // if this cell has been already dealt with by another segment, 
            // proceed to the next cell
            // note: the subsequent algorithms have to be based on an untainted cell ..
            //
            if (viennadata::find<viennamesh::data::orient, int>()(*cit))
            {
               continue;
            }
            else
            {
               seed_cell = *cit;
            #ifdef MESH_ADAPTOR_DEBUG         
               std::cout << "  detected seed cell:  ";
               VertexOnCellContainer vertices_for_cell = viennagrid::ncells<0>(*cit);
               for (VertexOnCellIterator vocit = vertices_for_cell.begin();
                   vocit != vertices_for_cell.end();
                   ++vocit)
               {
                  std::cout << vocit->getID() << " ";
               }            
               std::cout << std::endl;
            #endif
               break;
            }
         }
            
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
         
         PointType normal = viennagrid::cross_prod(vec1,vec2);
         
      #ifdef MESH_ADAPTOR_DEBUG
         std::cout << "  computed cell normal: " << normal << std::endl;
         std::cout << "  performing ray intersection tests .. " << std::endl;
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
         for (CellIterator cit = cells.begin(); cit != cells.end(); ++cit)
         {
            CellType & cell = *cit;

            // if the current cell under test is the seed cell, skip it
            // it does not make sense to test the intersection with the seed cell
            // as it surely does, and which is ofc expected.
            //
            if(cell.getID() == seed_cell.getID()) continue;
            
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
         #ifdef MESH_ADAPTOR_DEBUG
            std::cout << "  seed cell normal vector points inwards - correcting .." << std::endl;
         #endif             
            VertexType *vertices[viennagrid::traits::subcell_desc<CellTag, 0>::num_elements];  

            boost::array<index_type, CELLSIZE>    temp_cell;
            vi = 0;
            VertexOnCellContainer vertices_for_cell = viennagrid::ncells<0>(seed_cell);
            for (VertexOnCellIterator vocit = vertices_for_cell.begin();
                vocit != vertices_for_cell.end();
                ++vocit)
            {
               temp_cell[vi++] = vocit->getID();
            }            

            // return the orientation by using reversed indices 
            // of the original cell
            //
            vertices[0] = &(domain->vertex(temp_cell[2]));
            vertices[1] = &(domain->vertex(temp_cell[1]));
            vertices[2] = &(domain->vertex(temp_cell[0]));

            seed_cell.setVertices(vertices);
            seed_cell.fill(*domain);
            
            norm_vector_corrections++;
         }
      #ifdef MESH_ADAPTOR_DEBUG
         else {
            std::cout << "  seed cell normal vector points outwards .." << std::endl;
         }
      #endif             
         

         // initiate an orientation quantity on all cells
         //
         for (CellIterator cit = cells.begin(); cit != cells.end(); ++cit)
         {
           if (!viennadata::find<viennamesh::data::orient, int>()(*cit))
             viennadata::access<viennamesh::data::orient, int>()(*cit) = 0; 
         }
         // the first cell is already oriented
         //
         viennadata::access<viennamesh::data::orient, int>()(seed_cell) = 1; 

         // start the recursive orientation algorithm, originating from the seed cell
         // --> all connected cells should be oriented consistantly with the seed cell 
         //
         std::size_t corrected_cells = 0;
         std::size_t recursion_depth = 0;
         
         cell_orienter_recursive(seg, seed_cell, corrected_cells, recursion_depth);

         std::size_t consistant = 0;
         std::size_t leftovers = 0;
         std::size_t errors = 0;
         for (CellIterator cit = cells.begin(); cit != cells.end(); ++cit)
         {
            if(viennadata::access<viennamesh::data::orient, int>()(*cit) == 0) 
               leftovers++;
            else
            if(viennadata::access<viennamesh::data::orient, int>()(*cit) == 1) 
               consistant++;
            else
            if(viennadata::access<viennamesh::data::orient, int>()(*cit) == -1) 
               errors++;
         }      

         global_consistant       += consistant;
         global_leftovers        += leftovers;
         global_errors           += errors;
         global_corrected_cells  += corrected_cells;
      #ifdef MESH_ADAPTOR_DEBUG
         std::cout << "  finished segment - statistics:" << std::endl;
         std::cout << "      consistants:      " << consistant << std::endl;
         std::cout << "      cell-corrections: " <<  corrected_cells << std::endl;
         std::cout << "      leftovers:        " << leftovers << std::endl;
         std::cout << "      errors:           " << errors << std::endl;                  
      #endif                              
         if(errors != 0 || leftovers != 0)
            std::cerr << "## MeshAdaptor::"+id+" - segment orientation experienced errors" << std::endl;
            
//         // remove the orientation quantity of the cells of the current segment
//         //
//         for (CellIterator cit = cells.begin(); cit != cells.end(); ++cit)
//         {
//           viennadata::erase<std::string, int>("orient")(*cit); 
//         }            
      }
   #ifdef MESH_ADAPTOR_DEBUG
      std::cout << std::endl;
      std::cout << "------------------------------------------------------ " << std::endl;
   #endif
      if(global_errors != 0 || global_leftovers != 0)
         std::cerr << "## MeshAdaptor::"+id+" - ERROR: segment orientation failed" << std::endl;         
   #ifdef MESH_ADAPTOR_DEBUG
      else std::cout << "## MeshAdaptor::"+id+" - finished successfully!" << std::endl;
         
      std::cout << "------------------------------------------------------ " << std::endl;
      std::cout << "## MeshAdaptor::"+id+" - result statistics:" << std::endl;
      std::cout << "   seed-corrections: " <<  norm_vector_corrections << std::endl;
      std::cout << "   cell-corrections: " <<  global_corrected_cells << std::endl;
      std::cout << "   consistants:      " << global_consistant << std::endl;
      std::cout << "   leftovers:        " << global_leftovers << std::endl;
      std::cout << "   errors:           " << global_errors << std::endl;                  
      std::cout << "------------------------------------------------------ " << std::endl;      
   #endif                                    
      return domain;
   }
   // -------------------------------------------------------------------------------------
   
   // -------------------------------------------------------------------------------------
   template<typename SegmentT, typename CellT>
   bool cell_orienter_recursive(SegmentT& segment, CellT& cell, std::size_t& corrected_cells, std::size_t& recursion_depth)
   {
      //std::cout << "cell recursion for cell: " << cell.getID() << std::endl;
   
      // investigate how many neighbour cells have been dealt with so far ..
      //
      long number_of_not_oriented_cells = 0;

      EdgeOnCellContainer edges = viennagrid::ncells<1>(cell);

      for (EdgeOnCellIterator eocit = edges.begin(); eocit != edges.end(); ++eocit)
      {
         CellOnEdgeContainer cells = viennagrid::ncells<CellTag::topology_level>(*eocit, segment);

         for (CellOnEdgeIterator coeit = cells.begin(); coeit != cells.end(); ++coeit)
         {         
            if(viennadata::access<viennamesh::data::orient, int>()(*coeit) == 0)
               number_of_not_oriented_cells++;
         }
      }                  
      // stop condition for recursion
      //
      if (number_of_not_oriented_cells == 0)
      {
         //std::cout << "  stopping recursion " << std::endl;
         return true;      
      }
      
      const std::size_t segment_id = segment.id();
      
      // orient the adjacent cells according to the given cell
      //
      for (EdgeOnCellIterator eocit = edges.begin(); eocit != edges.end(); ++eocit)
      {
         CellOnEdgeContainer cells = viennagrid::ncells<CellTag::topology_level>(*eocit, segment);

         for (CellOnEdgeIterator coeit = cells.begin(); coeit != cells.end(); ++coeit)
         {         
            // if this cell is the reference cell, skip ..
            //
            if( coeit->getID() == cell.getID() )   continue;
            
            // if this cell has already been dealt with regarding orientation ..
            // 
            if (viennadata::access<viennamesh::data::orient, int>()(*coeit) != 0)
            {
               // if this cell has already been dealt with by _another_ segment, 
               // record that the orientation has to be considered inverse for this segment
               //
               if ( (viennadata::find<viennamesh::data::seg_orient, viennamesh::data::seg_orient_map::type>()(*coeit)) &&
                    (viennadata::access<viennamesh::data::seg_orient, viennamesh::data::seg_orient_map::type>()(*coeit).find(segment_id) == 
                     viennadata::access<viennamesh::data::seg_orient, viennamesh::data::seg_orient_map::type>()(*coeit).end() ) )
               {
                  //std::cout << "  found already corrected cell .. " << std::endl;
                  viennadata::access<viennamesh::data::seg_orient, viennamesh::data::seg_orient_map::type>()(*coeit)[segment_id]= -1;                
               }
               continue;
            }
            
            if(is_consistant_orientation(*coeit, cell))
            {  
               //std::cout << "  is consitant" << std::endl;
               viennadata::access<viennamesh::data::orient, int>()(*coeit) = 1;
               viennadata::access<viennamesh::data::seg_orient, viennamesh::data::seg_orient_map::type>()(*coeit)[segment_id]= 1; 
            }
            // repair the orientation
            else
            {  
               //std::cout << "  is NOT consitant" << std::endl;
               boost::array<index_type, CELLSIZE>    temp_cell;
               std::size_t vi = 0;
               VertexOnCellContainer vertices_for_cell = viennagrid::ncells<0>(*coeit);
               for (VertexOnCellIterator vocit = vertices_for_cell.begin();
                   vocit != vertices_for_cell.end();
                   ++vocit)
               {
                  temp_cell[vi++] = vocit->getID();
               }                        
            
               VertexType *vertices[viennagrid::traits::subcell_desc<CellTag, 0>::num_elements];  
               vertices[0] = &(segment.get_domain().vertex(temp_cell[2]));               
               vertices[1] = &(segment.get_domain().vertex(temp_cell[1]));
               vertices[2] = &(segment.get_domain().vertex(temp_cell[0]));
               coeit->setVertices(vertices);
               coeit->fill(segment.get_domain());               
               
               // check again to verify ..
               //
               if(is_consistant_orientation(*coeit, cell))
               { 
                  //std::cout << "sucessfully corrected a cells orientation . " << std::endl;
                  viennadata::access<viennamesh::data::orient, int>()(*coeit) = 1;
                  viennadata::access<viennamesh::data::seg_orient, viennamesh::data::seg_orient_map::type>()(*coeit)[segment_id]= 1;                   
                  corrected_cells++;
               }               
               else
               {
                  viennadata::access<viennamesh::data::orient, int>()(*coeit) = -1;
                  std::cerr << "ERROR @ cell-orienter-recursive: could not repair orientation .." << std::endl;
               }
            }
         }
      }                        
      
      //viennagrid::io::export_vtk(*domain, "recursion_"+boost::lexical_cast<std::string>(recursion_depth));         
      
      // by now, all adjacent cells have been dealt with
      // step into next recursion level
      //
      for (EdgeOnCellIterator eocit = edges.begin(); eocit != edges.end(); ++eocit)
      {
         CellOnEdgeContainer cells = viennagrid::ncells<CellTag::topology_level>(*eocit, segment);

         for (CellOnEdgeIterator coeit = cells.begin(); coeit != cells.end(); ++coeit)
         {         
            if( coeit->getID() == cell.getID() )   continue;
            cell_orienter_recursive(segment, *coeit, corrected_cells, recursion_depth);            
         }
      }                  
      return true; // TODO is this right?
   }
   // -------------------------------------------------------------------------------------      
   
   // -------------------------------------------------------------------------------------         
   template<typename CellT>
   bool is_consistant_orientation(CellT & c1, CellT & c2)
   {
      // transfer input viennagrid cells to boost arrays
      // to support a more convienient vertex access level
      //
      boost::array<index_type, CELLSIZE>  bc1, bc2;
      std::size_t k = 0;
      VertexOnCellContainer vertices_c1 = viennagrid::ncells<0>(c1);
      for (VertexOnCellIterator vocit = vertices_c1.begin();
          vocit != vertices_c1.end();
          ++vocit)
      {
         bc1[k++] = vocit->getID();
      }               
      k = 0;
      VertexOnCellContainer vertices_c2 = viennagrid::ncells<0>(c2);
      for (VertexOnCellIterator vocit = vertices_c2.begin();
          vocit != vertices_c2.end();
          ++vocit)
      {
         bc2[k++] = vocit->getID();
      }               

      // checks, if two out of n points are oriented consistently
      //   
      std::size_t i, j;
      for(i = 0; i <= 2; i++)
      {
         for(j = 0; j <= 2; j++)
         {
            if (bc1[(i+1)%3]  == bc2[j] &&    
                bc1[i]        == bc2[(j+1)%3])
            {
               return true;
            }
         }
      }
      return false;
   }
   // -------------------------------------------------------------------------------------      
   
   std::string id;
};

} // end namespace viennamesh

#endif










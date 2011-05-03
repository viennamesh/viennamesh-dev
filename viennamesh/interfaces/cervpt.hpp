/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_INTERFACE_CERVPT_HPP
#define VIENNAMESH_INTERFACE_CERVPT_HPP

// *** system includes
#include <vector>
#include <map>

// *** boost includes
#include <boost/array.hpp>
#include <boost/assert.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/static_assert.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/fusion/include/has_key.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/make_map.hpp>
#include <boost/shared_ptr.hpp>

// *** cervpt includes
#include "cervpt/poly2tri.hpp" 

// *** vienna includes
#include "viennautils/dumptype.hpp"
#include "viennautils/contio.hpp"
#include "viennamesh/interfaces/base.hpp"
#include "viennamesh/tags.hpp"
#include "viennagrid/domain.hpp"
 
//#define MESH_KERNEL_DEBUG
//#define MESH_KERNEL_DEBUG_FULL

namespace viennamesh {

/** @brief tag-dispatched mesher kernel specialization for Tetgen
*/   

template<>
struct mesh_kernel <viennamesh::tag::cervpt>
{
   // -------------------------------------------------------------------------------------
   typedef long       integer_type;
   // -------------------------------------------------------------------------------------   

   typedef viennagrid::domain<viennagrid::config::triangular_3d>     domain_type;
   typedef boost::shared_ptr<domain_type>                            domain_ptr_type;
   typedef domain_type::config_type                                  domain_configuration_type;
   typedef domain_configuration_type::numeric_type                   numeric_type;
   typedef domain_configuration_type::cell_tag                       cell_tag;
   
   typedef viennagrid::result_of::ncell_type<domain_configuration_type, 0>::type                            vertex_type;   
   typedef viennagrid::result_of::ncell_type<domain_configuration_type, cell_tag::topology_level>::type     cell_type;   

   static const int DIMG = domain_configuration_type::dimension_tag::value;
   static const int DIMT = domain_configuration_type::cell_tag::topology_level;

   typedef viennamesh::tag::mesh_kernel            datastructure_type; // type is used for viennamesh::traits   
   
   typedef domain_ptr_type                         result_type;
   // -------------------------------------------------------------------------------------   
   /** @brief constructor expects a ViennaMesh::wrapped datastructure
   */      
   mesh_kernel() 
   {
      mesh_kernel_id = "CervPT";      
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - initiating" << std::endl;
   #endif      
   }
   // -------------------------------------------------------------------------------------
   
   // -------------------------------------------------------------------------------------   
   /** @brief destructor takes care of releasing mesh kernel memory
   */      
   ~mesh_kernel()
   {
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - shutting down" << std::endl;
   #endif
   }   
   // -------------------------------------------------------------------------------------

   // -------------------------------------------------------------------------------------   
   /** @brief functor expects a parameter set based on a boost::fusion::map
   */   
   template <typename DatastructureT>
   result_type operator()(DatastructureT& data) // default meshing
   {
      return (*this)(data, boost::fusion::make_map<tag::criteria>(tag::constrained_delaunay()));
   }   
   template<typename DatastructureT, typename ParametersMapT>
   result_type operator()(DatastructureT& data, ParametersMapT const& paras )  // TODO provide ct-test if fusion::map
   {
      // redirect to reference implementation 
      ParametersMapT paras_new(paras);
      return (*this)(data, paras_new);
   }
   template<typename DatastructureT, typename ParametersMapT>
   result_type operator()(DatastructureT& data, ParametersMapT & paras)   // TODO provide ct-test if fusion::map
   {  
      typedef typename DatastructureT::segment_iterator  vmesh_segment_iterator;
      typedef typename DatastructureT::cell_type         vmesh_cell_type;
      typedef typename DatastructureT::cell_iterator     vmesh_cell_iterator;      
   
      mesh_kernel_id = "cervpt";      
      
      // TODO provide conecept check mechanism - is it a ViennaMesh::Wrapper ?
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - initiating" << std::endl;
   #endif
   
      domain_ptr_type domain(new domain_type);   
   
   #ifdef MESH_KERNEL_DEBUG
      std::cout << std::endl;
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - processing geometry" << std::endl;
   #endif
      typedef typename DatastructureT::geometry_iterator geometry_iterator;
      for(geometry_iterator iter = data.geometry_begin();
          iter != data.geometry_end(); iter++)
      {
      #ifdef MESH_KERNEL_DEBUG_FULL
          std::cout << "## MeshKernel::"+mesh_kernel_id+" - adding point " << 
             std::distance(data.geometry_begin(), iter) << " : " << *iter << std::endl;
      #endif   
         
         p2tri.add_point(*iter);
      }      
   
   #ifdef MESH_KERNEL_DEBUG
      std::cout << std::endl;   
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - processing constraints" << std::endl;
   #endif
      size_t si = 0;

      domain->reserve_vertices(std::distance(data.geometry_begin(), data.geometry_end()));

      //
      // loading geometry data
      //     NOTE --> as no new points are introduced, we can directly 
      //     copy the input geometry space
      //

      #ifdef MESH_KERNEL_DEBUG
         std::cout << "## MeshKernel::"+mesh_kernel_id+" - transferring geometry" << std::endl;
      #endif                              

      std::size_t pi = 0;
      typedef typename DatastructureT::geometry_iterator geometry_iterator;
      for(geometry_iterator iter = data.geometry_begin();
          iter != data.geometry_end(); iter++)
      {
         numeric_type   coords[DIMG];
         vertex_type    vertex;
      
         coords[0] = (*iter)[0];
         coords[1] = (*iter)[1];
         coords[2] = (*iter)[2];
         
         vertex.getPoint().setCoordinates(coords);         
         vertex.setID(pi++);
         domain->add(vertex);
      }   
      
      domain->create_segments(std::distance(data.segment_begin(),data.segment_end()));      

      typedef std::vector<cell_type>                   cell_cont_type;
      typedef std::map<std::size_t, cell_cont_type >   segment_cell_map_type;
      segment_cell_map_type segment_cell_map;
      std::size_t global_cell_size = 0;

   #ifdef MESH_KERNEL_DEBUG_FULL
      size_t cell_cnt = 0;
   #endif
      for(vmesh_segment_iterator seg_iter = data.segment_begin();
         seg_iter != data.segment_end(); seg_iter++)
      {  
         for(vmesh_cell_iterator cit = (*seg_iter).cell_begin();
             cit != (*seg_iter).cell_end(); cit++)
         {  
            vmesh_cell_type cell = *cit;

         #ifdef MESH_KERNEL_DEBUG_FULL
            std::cout << "## MeshKernel::"+mesh_kernel_id+" - adding constraint   " << 
               cell_cnt << " : ";
            for(size_t i = 0; i < cell.size(); i++)  
               std::cout << cell[i] << " ";
            std::cout << std::endl;
            cell_cnt++;
         #endif               
            p2tri.add_constraint(cell);
         }
         


      #ifdef MESH_KERNEL_DEBUG
         std::cout << "## MeshKernel::"+mesh_kernel_id+" - meshing:" << std::endl;
         std::cout << "  input point size:  " << p2tri.point_size() << std::endl;
         std::cout << "  input const size:  " << p2tri.constraint_size() << std::endl;      
         std::cout << "## MeshKernel::"+mesh_kernel_id+" starting mesh generation " << std::endl;               
      #endif         
      
         p2tri.mesh();
         
         if ( !p2tri.triangle_size())      
            std::cout << "\t::ERROR: no mesh has been created" << std::endl;
         
      #ifdef MESH_KERNEL_DEBUG
         std::cout << "## MeshKernel::"+mesh_kernel_id+" - finished:" << std::endl;
         std::cout << "## MeshKernel::"+mesh_kernel_id+" - extracting topology" << std::endl;
      #endif                              

        
         typedef typename cervpt::poly2tri::poly_triangle_cont_map_type::iterator   polygon_iter_type;
         typedef typename cervpt::poly2tri::triangle_container_type::iterator       triangle_iter_type;
         
         for(polygon_iter_type polyiter = p2tri.triangles_cont().begin(); 
             polyiter != p2tri.triangles_cont().end(); polyiter++)
         {
            for(triangle_iter_type triiter = polyiter->second.begin();
                triiter != polyiter->second.end(); triiter++)
            {
                vertex_type *vertices[viennagrid::subcell_traits<cell_tag, 0>::num_elements];        
                
                for(std::size_t ci = 0; ci < triiter->size(); ci++)
                {
                  vertices[ci] = &(domain->vertex((*triiter)[ci]));
                }

               cell_type cell;
               cell.setVertices(vertices);
               segment_cell_map[si].push_back(cell);
               global_cell_size++;
            }
         }

         // reset the input topolgy and the output  from the previous run
         // 
         p2tri.reset_topology();
         
         si++;
      }                  
      

      // now that all segments and cells have been read, and due to that 
      // we are aware of how many cells in total there are, 
      // we can actually create the cells within the domain
      std::cout << "global cell size: " << global_cell_size << std::endl;
      domain->reserve_cells(global_cell_size);

      for(typename segment_cell_map_type::iterator sit = segment_cell_map.begin();
          sit != segment_cell_map.end(); sit++)
      {
         for(typename cell_cont_type::iterator cit = sit->second.begin();
             cit != sit->second.end(); cit++)
         {
            domain->segment(sit->first).add(*cit);            
         }
      }
            
      return domain;
   }
   
   // -------------------------------------------------------------------------------------

private:   
   // -------------------------------------------------------------------------------------   
  
   std::string    mesh_kernel_id;
   std::string    options;
                  
   cervpt::poly2tri  p2tri;
};

} // end namespace viennamesh

#endif


/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

// *** vienna includes
#include "viennamesh/wrapper.hpp"

// *** local includes
#include "cervpt.hpp"

// *** cervpt includes
#include "cervpt/poly2tri.hpp" 
 
namespace viennamesh {

mesh_kernel<viennamesh::tag::cervpt>::mesh_kernel()
{
   mesh_kernel_id = "CervPT";      
#ifdef MESH_KERNEL_DEBUG
   std::cout << "## MeshKernel::"+mesh_kernel_id+" - initiating" << std::endl;
#endif
}
// --------------------------------------------------------------------------
mesh_kernel<viennamesh::tag::cervpt>::~mesh_kernel()
{
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - shutting down" << std::endl;
   #endif
}

template<typename DatastructureT>
mesh_kernel<viennamesh::tag::cervpt>::result_type 
mesh_kernel<viennamesh::tag::cervpt>::operator()(DatastructureT& data)
{
   cervpt::poly2tri  p2tri;

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
   //         numeric_type   coords[DIMG];      
   //         coords[0] = (*iter)[0];
   //         coords[1] = (*iter)[1];
   //         coords[2] = (*iter)[2];

    vertex_type    vertex;         
    vertex.getPoint()[0] = (*iter)[0];
    vertex.getPoint()[1] = (*iter)[1];         
    vertex.getPoint()[2] = (*iter)[2];         
    vertex.id(pi++);
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


    typedef cervpt::poly2tri::poly_triangle_cont_map_type::iterator   polygon_iter_type;
    typedef cervpt::poly2tri::triangle_container_type::iterator       triangle_iter_type;
    
    for(polygon_iter_type polyiter = p2tri.triangles_cont().begin(); 
        polyiter != p2tri.triangles_cont().end(); polyiter++)
    {
       for(triangle_iter_type triiter = polyiter->second.begin();
           triiter != polyiter->second.end(); triiter++)
       {
           vertex_type *vertices[viennagrid::topology::subcell_desc<cell_tag, 0>::num_elements];        
           
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
   //std::cout << "global cell size: " << global_cell_size << std::endl;
   domain->reserve_cells(global_cell_size);

   for(segment_cell_map_type::iterator sit = segment_cell_map.begin();
     sit != segment_cell_map.end(); sit++)
   {
    for(cell_cont_type::iterator cit = sit->second.begin();
        cit != sit->second.end(); cit++)
    {
       domain->segment(sit->first).add(*cit);            
    }
   }
       
   return domain;
}

// -----------------------------------------------------------------------------
// 
// explicit declarations for the template functions
// 
template mesh_kernel<viennamesh::tag::cervpt>::result_type 
mesh_kernel<viennamesh::tag::cervpt>::operator()(viennamesh::wrapper<viennamesh::tag::bnd, viennautils::io::bnd_reader>& data);

template mesh_kernel<viennamesh::tag::cervpt>::result_type 
mesh_kernel<viennamesh::tag::cervpt>::operator()(viennamesh::wrapper<viennamesh::tag::hin, viennautils::io::hin_reader>& data);

template mesh_kernel<viennamesh::tag::cervpt>::result_type 
mesh_kernel<viennamesh::tag::cervpt>::operator()(viennamesh::wrapper<viennamesh::tag::viennagrid, viennagrid::domain<viennagrid::config::triangular_3d> >& data);
// -----------------------------------------------------------------------------

} // end namespace viennamesh



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

// *** local includes
#include "viennamesh/adaptation/hull_quality.hpp"

// *** vienna includes
#include "viennamesh/data.hpp"
#include "viennadata/api.hpp"
#include "viennagrid/algorithm/cell_normals.hpp"

// *** boost includes
#include <boost/make_shared.hpp>
#include <boost/array.hpp>

// *** vgmodeler includes
#include "vgmodeler/hull_adaption/vgmodeler.hpp"

// *** gsse 01 includes
#include "gsse/domain.hpp"

namespace viennamesh {

// --------------------------------------------------------------------------
mesh_adaptor<viennamesh::tag::hull_quality>::mesh_adaptor()
{
   id = "HullQuality";      
#ifdef MESH_KERNEL_DEBUG
   std::cout << "## MeshAdaptor::"+id+" - initiating" << std::endl;
#endif
}
// --------------------------------------------------------------------------
mesh_adaptor<viennamesh::tag::hull_quality>::~mesh_adaptor()
{
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshAdaptor::"+id+" - shutting down" << std::endl;
   #endif
}
// --------------------------------------------------------------------------
mesh_adaptor<viennamesh::tag::hull_quality>::result_type 
mesh_adaptor<viennamesh::tag::hull_quality>::operator()(domain_type& domain)
{
   // forwarding to main implementation
   return (*this)(boost::make_shared<domain_type>(domain));
}
// --------------------------------------------------------------------------
mesh_adaptor<viennamesh::tag::hull_quality>::result_type 
mesh_adaptor<viennamesh::tag::hull_quality>::operator()(input_type domain)
{
#ifdef MESH_ADAPTOR_DEBUG
   std::cout << "## MeshAdaptor::"+id+" - starting up .." << std::endl;
#endif            

#ifdef MESH_ADAPTOR_DEBUG
   std::cout << "## MeshAdaptor::"+id+" - loading hull elements" << std::endl;
#endif       
   
   vgmodeler::hull_adaptor vghull;

   for (std::size_t si = 0; si < domain->segment_size(); ++si)
   {
      SegmentType & seg = domain->segment(si);
      CellContainer cells = viennagrid::ncells<CellTag::topology_level>(seg);

      for (CellIterator cit = cells.begin(); cit != cells.end(); ++cit)
      {
         boost::array<PointType,CELLSIZE>     triangle;
         std::size_t vi = 0;       
         VertexOnCellContainer vertices_for_cell = viennagrid::ncells<0>(*cit);
         for (VertexOnCellIterator vocit = vertices_for_cell.begin();
             vocit != vertices_for_cell.end();
             ++vocit)
         {
            triangle[vi++] = vocit->getPoint();
         }
         
         if(viennadata::access<viennamesh::data::seg_orient, viennamesh::data::seg_orient_map::type>()(*cit)[si] == -1)
         {
            std::reverse(triangle.begin(), triangle.end());
         }
         
         vghull.add_hull_element(triangle, 
            viennadata::access<
               viennagrid::seg_cell_normal_tag, viennagrid::seg_cell_normal_data::type
            >()(*cit)[si]); 
         
      }
   }
   
#ifdef MESH_ADAPTOR_DEBUG
   std::cout << "## MeshAdaptor::"+id+" - adapting mesh .." << std::endl;
#endif       

   input_type new_domain(new domain_type);   
   vghull.process(*domain, *new_domain);

   return new_domain;
}
// --------------------------------------------------------------------------
} // end namespace viennamesh



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
#include "viennamesh/keys.hpp"
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
   id = viennamesh::key::hull_quality;      
   
   vghull = new vgmodeler::hull_adaptor;
   
#ifdef MESH_KERNEL_DEBUG
   std::cout << "## MeshAdaptor::"+id+" - initiating" << std::endl;
#endif
}
// --------------------------------------------------------------------------
mesh_adaptor<viennamesh::tag::hull_quality>::~mesh_adaptor()
{
   delete vghull;
   
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshAdaptor::"+id+" - shutting down" << std::endl;
   #endif
}
// --------------------------------------------------------------------------
void mesh_adaptor<viennamesh::tag::hull_quality>::set_maxsize(double size)
{
   vghull->maxsize() = size;
}
double mesh_adaptor<viennamesh::tag::hull_quality>::get_maxsize()
{
   return vghull->maxsize();
}
// --------------------------------------------------------------------------
void mesh_adaptor<viennamesh::tag::hull_quality>::set_minsize(double size)
{
   vghull->minsize() = size;
}
double mesh_adaptor<viennamesh::tag::hull_quality>::get_minsize()
{
   return vghull->minsize();
}
// --------------------------------------------------------------------------
void mesh_adaptor<viennamesh::tag::hull_quality>::set_maxangle(double angle)
{
   vghull->maxangle() = angle;
}
double mesh_adaptor<viennamesh::tag::hull_quality>::get_maxangle()
{
   return vghull->maxangle();
}
// --------------------------------------------------------------------------
void mesh_adaptor<viennamesh::tag::hull_quality>::set_grading(double grading)
{
   vghull->grading() = grading;
}
double mesh_adaptor<viennamesh::tag::hull_quality>::get_grading()
{
   return vghull->grading();
}
// --------------------------------------------------------------------------
void mesh_adaptor<viennamesh::tag::hull_quality>::assign(viennamesh::config::set const& paraset)
{
   typedef viennamesh::config::query::adaptor_maxsize       adaptor_maxsize_query_type;
   if(adaptor_maxsize_query_type::available(paraset, id))
   {
      adaptor_maxsize_query_type::result_type size = adaptor_maxsize_query_type::eval(paraset, id);
      if(size > 0) this->set_maxsize(size);
      else
      {
         viennautils::msg::warning("## MeshAdaptor::"+id+" max-size out of range - using default value ");
         viennautils::msg::warning("   valid range:   > 0.0");         
         viennautils::msg::warning("   default value: 1E5"); 
         this->set_maxsize(1E5);
      }
   }
   
   typedef viennamesh::config::query::adaptor_minsize       adaptor_minsize_query_type;
   if(adaptor_minsize_query_type::available(paraset, id))
   {
      adaptor_minsize_query_type::result_type size = adaptor_minsize_query_type::eval(paraset, id);
      if(size >= 0) this->set_minsize(size);
      else
      {
         viennautils::msg::warning("## MeshAdaptor::"+id+" min-size out of range - using default value ");
         viennautils::msg::warning("   valid range:   >= 0.0");                  
         viennautils::msg::warning("   default value: 0");          
         this->set_maxsize(0);
      }      
   }   
   
   typedef viennamesh::config::query::adaptor_maxangle       adaptor_maxangle_query_type;
   if(adaptor_maxangle_query_type::available(paraset, id))
   {
      adaptor_maxangle_query_type::result_type angle = adaptor_maxangle_query_type::eval(paraset, id);
      if((angle >= 150) && (angle <= 180)) this->set_maxangle(angle);
      else
      {
         viennautils::msg::warning("## MeshAdaptor::"+id+" max-angle out of range - using default value ");
         viennautils::msg::warning("   valid range:   150.0 <= x <= 180.0");                  
         viennautils::msg::warning("   default value: 175");          
         this->set_maxsize(175);
      }            
   }   
   
   typedef viennamesh::config::query::adaptor_grading    adaptor_grading_query_type;
   if(adaptor_grading_query_type::available(paraset, id))
   {   
      adaptor_grading_query_type::result_type grading = adaptor_grading_query_type::eval(paraset, id);
      if((grading > 0) && (grading <= 1.0)) this->set_grading(grading);
      else
      {
         viennautils::msg::warning("## MeshAdaptor::"+id+" grading out of range - using default value ");
         viennautils::msg::warning("   valid range:   0.0 < x <= 1.0");              
         viennautils::msg::warning("   default value: 0.3");          
         this->set_maxsize(0.3);
      }                  
   }
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
   
   //vgmodeler::hull_adaptor vghull;

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
         
         vghull->add_hull_element(triangle, 
            viennadata::access<
               viennagrid::seg_cell_normal_tag, viennagrid::seg_cell_normal_data::type
            >()(*cit)[si]); 
      }
   }
   
#ifdef MESH_ADAPTOR_DEBUG
   std::cout << "## MeshAdaptor::"+id+" - adapting mesh .." << std::endl;
#endif       

   input_type new_domain(new domain_type);   
   vghull->process(*domain, *new_domain);

   return new_domain;
}
// --------------------------------------------------------------------------
} // end namespace viennamesh



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

#ifndef VIENNAMESH_ADAPTORS_CELLNORMALS_HPP
#define VIENNAMESH_ADAPTORS_CELLNORMALS_HPP


// *** local includes
#include "viennamesh/adaptation/base.hpp"
#include "viennamesh/data.hpp"

// *** vienna includes
#include "viennagrid/domain.hpp"
#include "viennagrid/algorithm/cell_normals.hpp"
#include "viennadata/api.hpp"

// *** boost includes
#include <boost/make_shared.hpp>

//#define MESH_ADAPTOR_DEBUG_FULL

namespace viennamesh {

template <>
struct mesh_adaptor <viennamesh::tag::cell_normals>
{
   typedef viennagrid::domain<viennagrid::config::triangular_3d>     domain_type;
   typedef boost::shared_ptr< domain_type >                          input_type;
   typedef input_type                                                result_type;

   // -------------------------------------------------------------------------------------
   mesh_adaptor() : id("cell-normals") 
   {
   #ifdef MESH_ADAPTOR_DEBUG
      std::cout << "## MeshAdaptor::"+id+" - initiating .." << std::endl;
   #endif         
   }
   // -------------------------------------------------------------------------------------

   // -------------------------------------------------------------------------------------
   result_type operator()(domain_type domain)
   {
      // forwarding to main implementation
      return (*this)(boost::make_shared<domain_type>(domain));   
   }
   // -------------------------------------------------------------------------------------

   // -------------------------------------------------------------------------------------
   result_type operator()(input_type domain)
   {
   #ifdef MESH_ADAPTOR_DEBUG
      std::cout << "## MeshAdaptor::"+id+" - starting up .." << std::endl;
   #endif            

   #ifdef MESH_ADAPTOR_DEBUG
      std::cout << "   computing the cell normals" << std::endl;
   #endif            
      viennagrid::assign_cell_normals(*domain);

   #ifdef MESH_ADAPTOR_DEBUG
      std::cout << "   correcting interface cell normals" << std::endl;
   #endif            
      typedef domain_type::config_type                DomainConfiguration;
      typedef DomainConfiguration::cell_tag           CellTag;   
      typedef DomainConfiguration::numeric_type       NumericType;

      typedef domain_type::segment_type                                                                  SegmentType; 
      typedef viennagrid::result_of::ncell_type<DomainConfiguration, CellTag::topology_level>::type      CellType;
      typedef viennagrid::result_of::ncell_container<SegmentType, CellTag::topology_level>::type         CellContainer;          
      typedef viennagrid::result_of::iterator<CellContainer>::type                                       CellIterator;               

      // if there is only one segment, we are finished, 
      // as only interface cells between two or more segments have to be checked 
      //
      if(domain->segment_size() == 1) return domain;

      // traverse the segments
      //
      for (std::size_t si = 0; si < domain->segment_size(); ++si)
      {
         SegmentType & seg = domain->segment(si);

         // traverse the cells of this segment
         //
         CellContainer cells = viennagrid::ncells<CellTag::topology_level>(seg);
         
         for (CellIterator cit = cells.begin(); cit != cells.end(); ++cit)
         {
            if(viennadata::access<viennamesh::data::seg_orient, viennamesh::data::seg_orient_map::type>()(*cit)[si] == -1)
            {
            #ifdef MESH_ADAPTOR_DEBUG_FULL
               std::cout << "      segment: " << si << " cell: " << cit->getID() << ": correcting .. " << std::endl;
            #endif            
//               std::cout << "         pre: " <<                viennadata::access<
//                  viennagrid::seg_cell_normal_tag,         // key-type
//                  viennagrid::seg_cell_normal_data::type   // data-type
//               >()(*cit)[si] << std::endl;
            
               viennadata::access<
                  viennagrid::seg_cell_normal_tag,         // key-type
                  viennagrid::seg_cell_normal_data::type   // data-type
               >()(*cit)[si] *= -1;

//               std::cout << "         post: " <<                viennadata::access<
//                  viennagrid::seg_cell_normal_tag,         // key-type
//                  viennagrid::seg_cell_normal_data::type   // data-type
//               >()(*cit)[si] << std::endl;

            }
         }
      }

      return domain;
   }
   // -------------------------------------------------------------------------------------
   
   // -------------------------------------------------------------------------------------      
   std::string id;
   // -------------------------------------------------------------------------------------         
};

} // end namespace viennamesh

#endif










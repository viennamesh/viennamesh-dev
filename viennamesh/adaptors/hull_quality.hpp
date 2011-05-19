/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_ADAPTORS_HULLQUALITY_HPP
#define VIENNAMESH_ADAPTORS_HULLQUALITY_HPP


// *** vienna includes
#include "viennamesh/adaptors/base.hpp"

// *** boost includes

// *** vgmodeler includes
#include "vgmodeler/hull_adaption/vgmodeler.hpp"

//#define MESH_ADAPTOR_DEBUG


namespace viennamesh {

template <>
struct mesh_adaptor <viennamesh::tag::hull_quality>
{

   typedef viennagrid::domain<viennagrid::config::triangular_3d>     domain_type;
   typedef boost::shared_ptr< domain_type >                          input_type;
   typedef input_type                                                result_type;

   typedef domain_type::config_type                     DomainConfiguration;

   typedef DomainConfiguration::numeric_type            CoordType;
   typedef DomainConfiguration::dimension_tag           DimensionTag;
   typedef DomainConfiguration::cell_tag                CellTag;

   typedef domain_type::segment_type                                                                  SegmentType;
   typedef viennagrid::result_of::ncell_container<SegmentType, CellTag::topology_level>::type         CellContainer;      
   typedef viennagrid::result_of::iterator<CellContainer>::type                                       CellIterator;         

   // -------------------------------------------------------------------------------------
   mesh_adaptor() : id("hullquality") 
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
   
      /*
      
      1. transfer geometry, topology to vgmodeler datastructure
      2. adapt hull
      3. writeback result
      
      */

      //vgmodeler::hull_adaptor    vghull;
      hull_adaptor    vghull;
      vghull.add_hull_element(double(3.0));

      for (std::size_t si = 0; si < domain->segment_size(); ++si)
      {
         #ifdef MESH_ADAPTOR_DEBUG
         std::cout << "## MeshAdaptor::"+id+" - processing segment: " << si << std::endl;
         #endif       

         SegmentType & seg = domain->segment(si);
         CellContainer cells = viennagrid::ncells<CellTag::topology_level>(seg);

         for (CellIterator cit = cells.begin(); cit != cells.end(); ++cit)
         {
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










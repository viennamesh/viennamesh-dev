/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_INTERFACE_TETGEN_HPP
#define VIENNAMESH_INTERFACE_TETGEN_HPP

// *** vienna includes
#include "viennamesh/generation/base.hpp"
#include "viennamesh/tags.hpp"
#include "viennamesh/wrapper.hpp"
#include "viennagrid/domain.hpp"
#include "viennautils/io/bnd.hpp"

// *** boost includes
#include <boost/fusion/include/has_key.hpp>
#include <boost/fusion/include/make_map.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

// forward declaration
//
struct tetgenio;

namespace viennamesh {

template<>
struct mesh_kernel <viennamesh::tag::tetgen>
{
   // --------------------------------------------------------------------------      
   typedef double          numeric_type;  
   typedef std::size_t     integer_type;

   typedef viennagrid::domain<viennagrid::config::tetrahedral_3d>    domain_type;
   typedef boost::shared_ptr<domain_type>                            domain_ptr_type;
   typedef domain_type::config_type                                  domain_configuration_type;
   typedef domain_configuration_type::cell_tag                       cell_tag;
   
   typedef viennagrid::result_of::ncell_type<domain_configuration_type, 0>::type                            vertex_type;   
   typedef viennagrid::result_of::ncell_type<domain_configuration_type, cell_tag::topology_level>::type     cell_type;   
   typedef viennagrid::result_of::ncell_range<domain_type, cell_tag::topology_level>::type              cell_container;
   typedef viennagrid::result_of::ncell_range<cell_type, 0>::type                                       vertex_on_cell_container_type;
   typedef viennagrid::result_of::iterator<vertex_on_cell_container_type>::type                             vertex_on_cell_iterator_type;

   static const int DIMG = domain_configuration_type::dimension_tag::value;
   static const int DIMT = domain_configuration_type::cell_tag::topology_level;   
   static const int CELLSIZE = DIMT+1;

   typedef viennamesh::mesh_kernel<viennamesh::tag::tetgen>          self_type;

   typedef boost::array<numeric_type, DIMG>                          point_type;
   // --------------------------------------------------------------------------

   typedef boost::fusion::result_of::make_map<
      viennamesh::tag::dim_geom, viennamesh::tag::dim_topo, viennamesh::tag::cell_type, viennamesh::tag::algorithm,       viennamesh::tag::criteria,       
      viennamesh::tag::three,    viennamesh::tag::three,    viennamesh::tag::simplex,   viennamesh::tag::incremental_delaunay, viennamesh::tag::conforming_delaunay
   >::type                                         properties_map_type;
   typedef viennamesh::tag::mesh_kernel            datastructure_type; 
   typedef domain_ptr_type                         result_type;
   // --------------------------------------------------------------------------         
   mesh_kernel();
  ~mesh_kernel();
   // --------------------------------------------------------------------------     
   template<typename DatastructureT>
   result_type operator()(DatastructureT& data);
   // --------------------------------------------------------------------------     
   result_type operator()(viennagrid::domain<viennagrid::config::triangular_3d>& hull_domain);    
   result_type operator()(boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_3d> > hull_domain); 
   // --------------------------------------------------------------------------     
   
private:   
   // --------------------------------------------------------------------------     
   void do_meshing(domain_ptr_type domain);
   // --------------------------------------------------------------------------     
   template<typename PointT>
   void find_point_in_segment(boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_3d> >       hull_domain,
                              typename viennagrid::domain<viennagrid::config::triangular_3d>::segment_type&    seg, 
                              PointT& pnt, std::size_t segid);

   template<typename DatastructureT>
   void find_point_in_segment(DatastructureT& data, 
                              typename DatastructureT::cell_complex_wrapper_type & cell_complex, 
                              point_type& pnt, std::size_t segid);
   // --------------------------------------------------------------------------     
   void transfer_to_domain(domain_ptr_type domain, tetgenio *mesh);
   // --------------------------------------------------------------------------     
   template<typename PointT>
   void addPoint(PointT const& pnt);
   template<typename PointT>
   void addPoint(PointT      & pnt);

   template<typename ConstraintT>
   void addConstraint(ConstraintT const& constraint);
   template<typename ConstraintT>
   void addConstraint(ConstraintT      & constraint);

   template<typename PointT>
   void addRegion(PointT const& pnt);
   template<typename PointT>
   void addRegion(PointT      & pnt);
   // --------------------------------------------------------------------------     
   void extendPoints();
   void extendFacets();
   void extendRegions();
   // --------------------------------------------------------------------------     
   tetgenio* init_kernel(tetgenio *io);
   void init();
   void clear();
   void reset();
   void freeMem();
   // --------------------------------------------------------------------------     
   tetgenio    *in; 
   tetgenio    *out;
   std::string mesh_kernel_id;
   std::string options;

   integer_type   pointlist_index,
                  constraint_list_index,
                  regionlist_index,
                  mesher_facet_index,
                  segment_index;   
   // --------------------------------------------------------------------------     
};

} // end namespace viennamesh





#endif 



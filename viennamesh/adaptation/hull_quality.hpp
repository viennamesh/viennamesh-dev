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
#include "viennamesh/adaptation/base.hpp"
#include "viennamesh/tags.hpp"
#include "viennamesh/config.hpp"
#include "viennagrid/domain.hpp"
#include "viennagrid/config/simplex.hpp"

// *** boost includes
#include <boost/shared_ptr.hpp>

// forward declaration
//
namespace vgmodeler {
struct hull_adaptor;
}

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
   typedef viennagrid::result_of::ncell_type<DomainConfiguration, CellTag::topology_level>::type      CellType;   
   typedef viennagrid::result_of::ncell_range<domain_type, 0>::type                               GeometryContainer;      
   typedef viennagrid::result_of::iterator<GeometryContainer>::type                                   GeometryIterator;         
   typedef viennagrid::result_of::ncell_range<SegmentType, CellTag::topology_level>::type         CellContainer;      
   typedef viennagrid::result_of::iterator<CellContainer>::type                                       CellIterator;         
   typedef viennagrid::result_of::ncell_range<CellType, 0>::type                                  VertexOnCellContainer;
   typedef viennagrid::result_of::iterator<VertexOnCellContainer>::type                               VertexOnCellIterator;         
   typedef viennagrid::result_of::point_type<DomainConfiguration>::type                               PointType;   

   static const int DIMT = DomainConfiguration::cell_tag::topology_level;   
   static const int CELLSIZE = DIMT+1;      

   // --------------------------------------------------------------------------         
   mesh_adaptor();
  ~mesh_adaptor();
   // --------------------------------------------------------------------------     
   void   set_maxsize(double size);
   double get_maxsize();   
   void   set_minsize(double size);
   double get_minsize();   
   void   set_grading(double grading);
   double get_grading();   
   void   set_maxangle(double size);
   double get_maxangle();      
   void   set_curvfac(double fac);
   double get_curvfac();      
   void   set_curvenable(int fac);
   int    get_curvenable();         
   void assign(viennamesh::config::set const& paraset);
   // --------------------------------------------------------------------------        
   result_type operator()(domain_type& domain);   
   result_type operator()(input_type domain);
   // --------------------------------------------------------------------------     
private:
   // --------------------------------------------------------------------------         
   vgmodeler::hull_adaptor*      vghull;    
   std::string id;
   // --------------------------------------------------------------------------     
};

} // end namespace viennamesh

#endif



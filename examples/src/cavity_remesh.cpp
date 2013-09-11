#include <iostream>


#include "viennamesh/algorithm/cgal_delaunay_tetrahedron_mesher.hpp"

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/domain/element_creation.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/cross_prod.hpp"

#include "viennagrid/io/poly_reader.hpp"

#include "viennamesh/algorithm/cgal_plc_mesher.hpp"
#include "viennagrid/domain/neighbour_iteration.hpp"
#include "viennagrid/algorithm/geometry.hpp"

#include "viennagrid/algorithm/seed_point_segmenting.hpp"


#include "viennamesh/algorithm/vgmodeler_hull_adaption.hpp"
#include "viennamesh/algorithm/netgen_tetrahedron_mesher.hpp"
#include "viennamesh/algorithm/tetgen_tetrahedron_mesher.hpp"


#include "viennamesh/statistics/element_metrics.hpp"










// template<typename DomainT, typename MarkedAccessorT>
// void mark_neighours_step( DomainT const & domain, MarkedAccessorT marked_cells )
// {
//   typedef typename viennagrid::result_of::cell_tag<DomainT>::type CellTag;
//   typedef viennagrid::vertex_tag ConnectorTag;
//   
//   
//   typedef typename viennagrid::result_of::cell<DomainT>::type CellType;
//   std::vector<bool> tmp_marker;
//   typename viennagrid::result_of::accessor<std::vector<bool>, CellType>::type tmp_accessor( tmp_marker );
// 
//   
//   typedef typename viennagrid::result_of::const_cell_range<DomainT>::type CellRangeType;
//   typedef typename viennagrid::result_of::iterator<CellRangeType>::type CellRangeIterator;
//   
//   
//   CellRangeType cells = viennagrid::elements(domain);
//   for (CellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
//   {
//     if ( marked_cells(*cit) )
//     {
//       tmp_accessor(*cit) = true;
//       
//       typedef typename viennagrid::result_of::const_neighbour_range<DomainT, CellTag, ConnectorTag>::type NeighbourRangeType;
//       typedef typename viennagrid::result_of::iterator<NeighbourRangeType>::type NeighbourRangeIterator;
// 
//       NeighbourRangeType neighbours = viennagrid::neighbour_elements<CellTag, ConnectorTag>( domain, cit.handle() );
//       for (NeighbourRangeIterator ncit = neighbours.begin(); ncit != neighbours.end(); ++ncit)
//       {
//         tmp_accessor(*ncit) = true;
//       }    
//     }    
//   }
//   
//   for (CellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
//   {
//     if (tmp_accessor(*cit))
//       marked_cells(*cit) = true;
//   }
// }
// 
// template<typename DomainT, typename MarkedAccessorT, typename ElementT>
// void mark_neighours( DomainT const & domain, MarkedAccessorT marked_cells, ElementT const & initial_element, unsigned int num_iterations )
// {
//   marked_cells(initial_element) = true;
//   for (unsigned int i = 0; i < num_iterations; ++i)
//     mark_neighours_step(domain, marked_cells);
// }



template<typename DomainT, typename MarkedAccessorT, typename PointT, typename NumericT>
void mark_neighours_radius( DomainT const & domain, MarkedAccessorT & marked_cells, PointT const & center, NumericT radius )
{
  typedef typename viennagrid::result_of::point<DomainT>::type DomainPointType;
  
  typedef typename viennagrid::result_of::const_cell_range<DomainT>::type CellRangeType;
  typedef typename viennagrid::result_of::iterator<CellRangeType>::type CellRangeIterator;
  
  CellRangeType cells_in_segment = viennagrid::elements(domain);
  for (CellRangeIterator cit = cells_in_segment.begin(); cit != cells_in_segment.end(); ++cit)
  {
    if (  (viennagrid::norm_2( center - viennagrid::point(domain, viennagrid::vertices(*cit)[0]) ) < radius) &&
          (viennagrid::norm_2( center - viennagrid::point(domain, viennagrid::vertices(*cit)[1]) ) < radius) &&
          (viennagrid::norm_2( center - viennagrid::point(domain, viennagrid::vertices(*cit)[2]) ) < radius) &&
          (viennagrid::norm_2( center - viennagrid::point(domain, viennagrid::vertices(*cit)[3]) ) < radius) )
      marked_cells(*cit) = true;
  }
}



template<typename VolumeDomainT, typename MarkedAccessorT, typename HullDomainT>
void extract_hull(VolumeDomainT const & volume_domain, MarkedAccessorT const marked_cells, HullDomainT & hull_domain)
{
  typedef typename viennagrid::result_of::cell_tag<VolumeDomainT>::type CellTag;
  typedef typename viennagrid::result_of::cell<VolumeDomainT>::type CellType;
  
  typedef typename viennagrid::result_of::vertex_handle<HullDomainT>::type HullVertexHandleType;
  
  typedef typename viennagrid::result_of::const_triangle_range<VolumeDomainT>::type TriangleRangeType;
  typedef typename viennagrid::result_of::iterator<TriangleRangeType>::type TriangleRangeIterator;
  
  TriangleRangeType hull_triangles = viennagrid::elements(volume_domain);
  for (TriangleRangeIterator tit = hull_triangles.begin(); tit != hull_triangles.end(); ++tit)
  {
    typedef typename viennagrid::result_of::const_coboundary_range<VolumeDomainT, viennagrid::triangle_tag, CellTag>::type CoboundaryCellRange;
    
    bool use = false;
    
    CoboundaryCellRange coboundary_cells = viennagrid::coboundary_elements<viennagrid::triangle_tag, CellTag>( volume_domain, tit.handle() );
    if ( coboundary_cells.size() == 1)
    {
      use = marked_cells( coboundary_cells[0] );
    }
    else if ( coboundary_cells.size() == 2)
    {
      use = marked_cells( coboundary_cells[0] ) ^ marked_cells( coboundary_cells[1] );
    }
    else
      std::cout << "Something went torribly wrong..." << std::endl;
        
    
    if ( use )
      viennagrid::copy_element( *tit, hull_domain );
  }
}

















template<typename InputDomainT, typename OutputDomainT>
void remesh_worst_element( InputDomainT const & input_domain, OutputDomainT & output_domain )
{
  output_domain.clear();
  
  typedef typename viennagrid::result_of::cell<InputDomainT>::type CellType;
  typedef typename viennagrid::result_of::const_cell_range<InputDomainT>::type CellRangeType;
  typedef typename viennagrid::result_of::iterator<CellRangeType>::type CellRangeIterator;
  typedef viennamesh::radius_edge_ratio_tag MetricTag;
//   typedef viennamesh::aspect_ratio_tag MetricTag;
//   typedef viennamesh::min_angle_tag MetricTag;
//   typedef viennamesh::min_dihedral_angle_tag MetricTag;
  
  CellRangeType cells = viennagrid::elements(input_domain);
  CellRangeIterator worst_element = viennamesh::worst_element<MetricTag>( input_domain );
  typename viennagrid::result_of::point<InputDomainT>::type center = viennagrid::centroid( *worst_element );
  
  std::vector<bool> cavity_marker;
  typename viennagrid::result_of::field<std::vector<bool>, CellType>::type cavity_marker_accessor( cavity_marker );
  
  mark_neighours_radius( input_domain, cavity_marker_accessor, center, 4.0 );
//   mark_neighours( domain, cavity_marker_accessor, *worst_element, 3 );
  


  typedef viennagrid::triangular_3d_domain CavityHullType;
  CavityHullType cavity_hull;
  
  extract_hull( input_domain, cavity_marker_accessor, cavity_hull );
  
  {        
    viennagrid::io::vtk_writer<CavityHullType> vtk_writer;
    vtk_writer(cavity_hull, "cavity_hull");
  }


  
  InputDomainT meshed_cavity;
  
  
  {
    viennamesh::result_of::settings<viennamesh::tetgen_tetrahedron_tag>::type settings;
    
//     settings.cell_size = 10.0;
    settings.cell_radius_edge_ratio = 1.2;    
    
    viennamesh::run_algo<viennamesh::tetgen_tetrahedron_tag>(cavity_hull, meshed_cavity, settings);
  }
  
  
  {
    viennagrid::io::vtk_writer<viennagrid::tetrahedral_3d_domain> vtk_writer;
    vtk_writer(meshed_cavity, "cavity_meshed");
  }

  

  unsigned int to_remesh_counter = 0;
  
  cells = viennagrid::elements(input_domain);
  for (CellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
  {
    if ( !cavity_marker_accessor(*cit) )
      viennagrid::copy_element( *cit, output_domain );
    else
      ++to_remesh_counter;
  }
  
  cells = viennagrid::elements(meshed_cavity);
  for (CellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
    viennagrid::copy_element( *cit, output_domain );

  
  cells = viennagrid::elements(output_domain);
  CellRangeIterator worst_new_element = viennamesh::worst_element<MetricTag>( output_domain );

  
  std::cout << "-------------------------------------------------------" << std::endl;
  std::cout << "Metric worst element before:     " << viennamesh::metric<MetricTag>(*worst_element) << std::endl;
  std::cout << "Metric worst after:              " << viennamesh::metric<MetricTag>(*worst_new_element) << std::endl;
  
  std::cout << "Volume original cube:            " << viennagrid::volume(input_domain) << std::endl;
  std::cout << "Volume partial remeshed cube:    " << viennagrid::volume(output_domain) << std::endl;
  
  std::cout << "Elements marked for remesh:      " << to_remesh_counter << std::endl;
  std::cout << "Number of remeshed elements:     " << viennagrid::cells(meshed_cavity).size() << std::endl;  
  
  
  {        
    viennagrid::io::vtk_writer<viennagrid::tetrahedral_3d_domain> vtk_writer;
    vtk_writer(output_domain, "cube_partial_remeshed");
  }
  
}












int main()
{
  viennagrid::plc_3d_domain plc_domain;
  
  
  viennagrid::io::poly_reader reader;
  reader(plc_domain, "../../examples/data/cube.poly");

  
  
  viennagrid::triangular_3d_domain hull;
  {
    viennamesh::result_of::settings<viennamesh::cgal_plc_3d_mesher_tag>::type plc_settings;

    viennamesh::run_algo< viennamesh::cgal_plc_3d_mesher_tag >( plc_domain, hull, plc_settings );

    {
        viennagrid::io::vtk_writer<viennagrid::triangular_3d_domain> vtk_writer;
        vtk_writer(hull, "meshed_plc_hull");
    }
  }


  
  
  
  viennagrid::tetrahedral_3d_domain domain;

  
//   {
//     viennamesh::result_of::settings<viennamesh::tetgen_tetrahedron_tag>::type settings;
//     
// //     settings.cell_size = 10.0;
//     settings.cell_radius_edge_ratio = 2.0;
// //     settings.facet_angle = 25.0;
//     
//     
//     viennamesh::run_algo<viennamesh::tetgen_tetrahedron_tag>(hull, domain, settings);
//     
//   }
  
  {    
    typedef viennagrid::result_of::point<viennagrid::triangular_3d_domain>::type point_type;
    viennagrid::triangular_hull_3d_segmentation triangulated_plc_segmentation(hull);
    
    std::vector< std::pair< int, point_type > > seed_points;
    seed_points.push_back( std::make_pair(0, point_type(0.0, 0.0, 0.0)) );

    viennagrid::mark_face_segments( hull, triangulated_plc_segmentation, seed_points.begin(), seed_points.end() );

    viennagrid::triangular_3d_domain oriented_adapted_hull_domain;
    viennagrid::triangular_hull_3d_segmentation oriented_adapted_hull_segmentation(oriented_adapted_hull_domain);
    viennamesh::result_of::settings<viennamesh::vgmodeler_hull_adaption_tag>::type vgm_settings;

    vgm_settings.cell_size = 1.0;

    viennamesh::run_algo< viennamesh::vgmodeler_hull_adaption_tag >( hull, triangulated_plc_segmentation,
                                                                      oriented_adapted_hull_domain, oriented_adapted_hull_segmentation,
                                                                      vgm_settings );    
    
    viennagrid::tetrahedral_3d_segmentation tetrahedron_segmentation(domain);
    viennamesh::result_of::settings<viennamesh::netgen_tetrahedron_tag>::type netgen_settings;

    netgen_settings.cell_size = 1.0;

    viennamesh::run_algo< viennamesh::netgen_tetrahedron_tag >( oriented_adapted_hull_domain, oriented_adapted_hull_segmentation,
                                                                domain, tetrahedron_segmentation,
                                                                netgen_settings );
  }
  
  
  
  viennagrid::tetrahedral_3d_domain domain1;
  
  remesh_worst_element(domain, domain1);
  remesh_worst_element(domain1, domain);
//   remesh_worst_element(domain, domain1);
  

}

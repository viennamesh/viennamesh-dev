#include <iostream>
#include <boost/concept_check.hpp>


#include "viennamesh/algorithm/cgal_delaunay_tetrahedron_mesher.hpp"

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/cross_prod.hpp"

#include "viennagrid/io/poly_reader.hpp"

#include "viennamesh/algorithm/cgal_plc_mesher.hpp"
#include "viennagrid/mesh/neighbour_iteration.hpp"
#include "viennagrid/algorithm/geometry.hpp"

#include "viennagrid/algorithm/seed_point_segmenting.hpp"


#include "viennamesh/algorithm/vgmodeler_hull_adaption.hpp"
#include "viennamesh/algorithm/netgen_tetrahedron_mesher.hpp"
#include "viennamesh/algorithm/tetgen_tetrahedron_mesher.hpp"
#include "viennamesh/algorithm/cgal_delaunay_tetrahedron_mesher.hpp"
#include "viennamesh/algorithm/extract_hull.hpp"


#include "viennamesh/statistics/element_metrics.hpp"










// template<typename MeshT, typename MarkedAccessorT>
// void mark_neighours_step( MeshT const & mesh, MarkedAccessorT marked_cells )
// {
//   typedef typename viennagrid::result_of::cell_tag<MeshT>::type CellTag;
//   typedef viennagrid::vertex_tag ConnectorTag;
//   
//   
//   typedef typename viennagrid::result_of::cell<MeshT>::type CellType;
//   std::vector<bool> tmp_marker;
//   typename viennagrid::result_of::accessor<std::vector<bool>, CellType>::type tmp_accessor( tmp_marker );
// 
//   
//   typedef typename viennagrid::result_of::const_cell_range<MeshT>::type CellRangeType;
//   typedef typename viennagrid::result_of::iterator<CellRangeType>::type CellRangeIterator;
//   
//   
//   CellRangeType cells = viennagrid::elements(mesh);
//   for (CellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
//   {
//     if ( marked_cells(*cit) )
//     {
//       tmp_accessor(*cit) = true;
//       
//       typedef typename viennagrid::result_of::const_neighbour_range<MeshT, CellTag, ConnectorTag>::type NeighbourRangeType;
//       typedef typename viennagrid::result_of::iterator<NeighbourRangeType>::type NeighbourRangeIterator;
// 
//       NeighbourRangeType neighbours = viennagrid::neighbour_elements<CellTag, ConnectorTag>( mesh, cit.handle() );
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
// template<typename MeshT, typename MarkedAccessorT, typename ElementT>
// void mark_neighours( MeshT const & mesh, MarkedAccessorT marked_cells, ElementT const & initial_element, unsigned int num_iterations )
// {
//   marked_cells(initial_element) = true;
//   for (unsigned int i = 0; i < num_iterations; ++i)
//     mark_neighours_step(mesh, marked_cells);
// }



template<typename MeshT, typename MarkedAccessorT, typename PointT, typename NumericT>
void mark_neighours_radius( MeshT const & mesh, MarkedAccessorT & marked_cells, PointT const & center, NumericT radius )
{
  typedef typename viennagrid::result_of::point<MeshT>::type MeshPointType;
  
  typedef typename viennagrid::result_of::const_cell_range<MeshT>::type CellRangeType;
  typedef typename viennagrid::result_of::iterator<CellRangeType>::type CellRangeIterator;
  
  CellRangeType cells_in_segment = viennagrid::elements(mesh);
  for (CellRangeIterator cit = cells_in_segment.begin(); cit != cells_in_segment.end(); ++cit)
  {
    if (  (viennagrid::norm_2( center - viennagrid::point(mesh, viennagrid::vertices(*cit)[0]) ) < radius) &&
          (viennagrid::norm_2( center - viennagrid::point(mesh, viennagrid::vertices(*cit)[1]) ) < radius) &&
          (viennagrid::norm_2( center - viennagrid::point(mesh, viennagrid::vertices(*cit)[2]) ) < radius) &&
          (viennagrid::norm_2( center - viennagrid::point(mesh, viennagrid::vertices(*cit)[3]) ) < radius) )
      marked_cells(*cit) = true;
  }
}



template<typename IntervalT, typename PinT>
struct pin
{
  pin() : value(0) {}
  pin(IntervalT lower_, IntervalT upper_) : lower(lower_), upper(upper_), value(0) {}
  
  IntervalT lower;
  IntervalT upper;
  PinT     value;
};

template<typename IntervalT, typename PinT>
struct histogram
{
  typedef pin<IntervalT, PinT> PinType;
  
//   histogram(IntervalT min, IntervalT max, int num_intervals)
//   { init(min, max, num_intervals); }
  
  void init(IntervalT min, IntervalT max, int num_intervals)
  {
    pins.clear();
    IntervalT pin_size = (max-min)/num_intervals;
    for (int i = 0; i < num_intervals; ++i, min += pin_size)
      pins.push_back( PinType(min, min+pin_size) );
    total_count = 0;
  }
  
  void operator() (IntervalT value)
  {
    for (typename std::vector<PinType>::iterator it = pins.begin(); it != pins.end(); ++it)
    {
      if ( (value >= it->lower) && (value < it->upper) )
        ++it->value;
    }
    ++total_count;
  }
  
  void print( std::ostream & stream ) const
  {
    for (typename std::vector<PinType>::const_iterator it = pins.begin(); it != pins.end(); ++it)
      stream << (it->lower + it->upper)/2.0 << " " << static_cast<double>(it->value)/static_cast<double>(total_count) << std::endl;
  }
  
  std::vector<PinType> pins;
  PinT total_count;
};









template<typename InputMeshT, typename OutputMeshT>
void remesh_worst_element( InputMeshT const & input_mesh, OutputMeshT & output_mesh, std::string const & name )
{
  output_mesh.clear();
  
  int histogram_pins = 1000;
  
  typedef typename viennagrid::result_of::cell<InputMeshT>::type CellType;
  typedef typename viennagrid::result_of::const_cell_range<InputMeshT>::type CellRangeType;
  typedef typename viennagrid::result_of::iterator<CellRangeType>::type CellRangeIterator;
  typedef viennamesh::radius_edge_ratio_tag MetricTag;
//   typedef viennamesh::aspect_ratio_tag MetricTag;
//   typedef viennamesh::min_angle_tag MetricTag;
//   typedef viennamesh::min_dihedral_angle_tag MetricTag;
  
  
  
  histogram<double,long> hist;
  hist.init(0.6, 1.6, histogram_pins);
  
  CellRangeType cells = viennagrid::elements(input_mesh);
  CellRangeIterator worst_element = viennamesh::worst_element<MetricTag>( input_mesh );
  typename viennagrid::result_of::point<InputMeshT>::type center = viennagrid::centroid( *worst_element );
  
  std::vector<double> metric_container;
  typename viennagrid::result_of::accessor< std::vector<double>, CellType >::type metric_accessor(metric_container);
  for (CellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
  {
    double metric = viennamesh::metric<MetricTag>(*cit);;
    metric_accessor(*cit) = metric;
    hist(metric);
  }
  
  std::cout << "-------------------------------------------------------" << std::endl;
  std::cout << "Before remeshing" << std::endl;
  std::ofstream hist_before( (name+"_hist_before").c_str() );
  hist.print(hist_before);
  std::cout << "-------------------------------------------------------" << std::endl;

  
  {        
    viennagrid::io::vtk_writer<InputMeshT> vtk_writer;
    viennagrid::io::add_scalar_data_on_cells(vtk_writer, metric_accessor, "metric");
    vtk_writer(input_mesh, name + "_original");
  }
  
  std::vector<bool> cavity_marker;
  typename viennagrid::result_of::field<std::vector<bool>, CellType>::type cavity_marker_accessor( cavity_marker );
  
  mark_neighours_radius( input_mesh, cavity_marker_accessor, center, 4.0 );
//   mark_neighours( mesh, cavity_marker_accessor, *worst_element, 3 );
  

  typedef viennagrid::triangular_3d_mesh CavityHullType;
  CavityHullType cavity_hull;
  
  viennamesh::extract_hull( input_mesh, cavity_marker_accessor, cavity_hull );
  
  {        
    viennagrid::io::vtk_writer<CavityHullType> vtk_writer;
    vtk_writer(cavity_hull, name + "_cavity_hull");
  }


  
  InputMeshT meshed_cavity;
  
  
  {
    viennamesh::result_of::settings<viennamesh::tetgen_tetrahedron_tag>::type settings;
    
//     settings.cell_size = 10.0;
    settings.cell_radius_edge_ratio = 1.05;
    
    viennamesh::run_algo<viennamesh::tetgen_tetrahedron_tag>(cavity_hull, meshed_cavity, settings);
  }
  
//   {
//     viennamesh::result_of::settings<viennamesh::cgal_delaunay_tetrahedron_tag>::type settings;
//     
// //     settings.cell_size = 100.0;
// //     settings.cell_radius_edge_ratio = 1.5;
//     settings.cell_radius_edge_ratio = 1.2;
//     
//     viennamesh::run_algo<viennamesh::cgal_delaunay_tetrahedron_tag>(cavity_hull, meshed_cavity, settings);
//   }
  
  
  {
    viennagrid::io::vtk_writer<viennagrid::tetrahedral_3d_mesh> vtk_writer;
    vtk_writer(meshed_cavity, name + "_cavity_meshed");
  }

  
  InputMeshT old_mesh_without_cavity;
  

  unsigned int to_remesh_counter = 0;
  
  cells = viennagrid::elements(input_mesh);
  for (CellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
  {
    if ( !cavity_marker_accessor(*cit) )
    {
      viennagrid::copy_element( *cit, output_mesh );
      viennagrid::copy_element( *cit, old_mesh_without_cavity );
    }
    else
      ++to_remesh_counter;
  }
  
  cells = viennagrid::elements(meshed_cavity);
  for (CellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
    viennagrid::copy_element( *cit, output_mesh );

  
  cells = viennagrid::elements(output_mesh);
  CellRangeIterator worst_new_element = viennamesh::worst_element<MetricTag>( output_mesh );

  
  histogram<double,long> hist2;
  hist2.init(0.6, 1.6, histogram_pins);
  
  std::vector<double> metric_container2;
  typename viennagrid::result_of::accessor< std::vector<double>, CellType >::type metric_accessor2(metric_container2);
  for (CellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
  {
    double metric = viennamesh::metric<MetricTag>(*cit);;
    metric_accessor2(*cit) = metric;
    hist2(metric);
  }
  
  std::cout << "-------------------------------------------------------" << std::endl;
  std::cout << "After remeshing" << std::endl;
  std::ofstream hist_after( (name+"_hist_after").c_str() );
  hist2.print(hist_after);
  std::cout << "-------------------------------------------------------" << std::endl;
  
  std::cout << "-------------------------------------------------------" << std::endl;
  std::cout << "Metric worst element before:     " << viennamesh::metric<MetricTag>(*worst_element) << std::endl;
  std::cout << "Metric worst after:              " << viennamesh::metric<MetricTag>(*worst_new_element) << std::endl;
  
  std::cout << "Volume original cube:            " << viennagrid::volume(input_mesh) << std::endl;
  std::cout << "Volume partial remeshed cube:    " << viennagrid::volume(output_mesh) << std::endl;
  
  std::cout << "Elements marked for remesh:      " << to_remesh_counter << std::endl;
  std::cout << "Number of remeshed elements:     " << viennagrid::cells(meshed_cavity).size() << std::endl;  
  
  {        
    viennagrid::io::vtk_writer<viennagrid::tetrahedral_3d_mesh> vtk_writer;
    vtk_writer(old_mesh_without_cavity, name + "_old_without_cavity");
  }
  
  {        
    viennagrid::io::vtk_writer<viennagrid::tetrahedral_3d_mesh> vtk_writer;
    vtk_writer(output_mesh, name + "_cube_partial_remeshed");
  }
  
}












int main()
{
  viennagrid::plc_3d_mesh plc_mesh;
  
  
  viennagrid::io::poly_reader reader;
  reader(plc_mesh, "../../examples/data/cube.poly");

  
  
  viennagrid::triangular_3d_mesh hull;
  {
    viennamesh::result_of::settings<viennamesh::cgal_plc_3d_mesher_tag>::type plc_settings;

    viennamesh::run_algo< viennamesh::cgal_plc_3d_mesher_tag >( plc_mesh, hull, plc_settings );

    {
        viennagrid::io::vtk_writer<viennagrid::triangular_3d_mesh> vtk_writer;
        vtk_writer(hull, "meshed_plc_hull");
    }
  }


  
  
  
  viennagrid::tetrahedral_3d_mesh mesh;

  
//   {
//     viennamesh::result_of::settings<viennamesh::tetgen_tetrahedron_tag>::type settings;
//     
// //     settings.cell_size = 10.0;
//     settings.cell_radius_edge_ratio = 2.0;
// //     settings.facet_angle = 25.0;
//     
//     
//     viennamesh::run_algo<viennamesh::tetgen_tetrahedron_tag>(hull, mesh, settings);
//     
//   }
  
  {    
    typedef viennagrid::result_of::point<viennagrid::triangular_3d_mesh>::type point_type;
    viennagrid::triangular_hull_3d_segmentation triangulated_plc_segmentation(hull);
    
    std::vector< std::pair< int, point_type > > seed_points;
    seed_points.push_back( std::make_pair(0, point_type(0.0, 0.0, 0.0)) );

    viennagrid::mark_face_segments( hull, triangulated_plc_segmentation, seed_points.begin(), seed_points.end() );

    viennagrid::triangular_3d_mesh oriented_adapted_hull_mesh;
    viennagrid::triangular_hull_3d_segmentation oriented_adapted_hull_segmentation(oriented_adapted_hull_mesh);
    viennamesh::result_of::settings<viennamesh::vgmodeler_hull_adaption_tag>::type vgm_settings;

    vgm_settings.cell_size = 1.0;

    viennamesh::run_algo< viennamesh::vgmodeler_hull_adaption_tag >( hull, triangulated_plc_segmentation,
                                                                      oriented_adapted_hull_mesh, oriented_adapted_hull_segmentation,
                                                                      vgm_settings );    
    
    viennagrid::tetrahedral_3d_segmentation tetrahedron_segmentation(mesh);
    viennamesh::result_of::settings<viennamesh::netgen_tetrahedron_tag>::type netgen_settings;

    netgen_settings.cell_size = 1.0;

    viennamesh::run_algo< viennamesh::netgen_tetrahedron_tag >( oriented_adapted_hull_mesh, oriented_adapted_hull_segmentation,
                                                                mesh, tetrahedron_segmentation,
                                                                netgen_settings );
  }
  
  
  
  viennagrid::tetrahedral_3d_mesh mesh1;
  
  remesh_worst_element(mesh, mesh1, "pass0");
  remesh_worst_element(mesh1, mesh, "pass1");
//   remesh_worst_element(mesh, mesh1, "pass2");
  

}
